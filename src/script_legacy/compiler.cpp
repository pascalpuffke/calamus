#include <assert.hpp>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <script_legacy/compiler.h>
#include <script_legacy/config.h>
#include <script_legacy/debug.h>
#include <script_legacy/memory.h>
#include <unordered_map>
#include <util/print.h>

namespace calamus::script {

struct Parser {
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;
};

enum class Precedence {
    None,
    Assignment, // =
    Or, // or
    And, // and
    Equality, // == !=
    Comparison, // < > <= >=
    Term, // + -
    Factor, // * /
    Unary, // ! -
    Call, // . ()
    Primary
};

typedef void (*ParseFn)(bool can_assign);

struct ParseRule {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};

struct Local {
    Token name;
    i32 depth;
    bool is_captured;
};

struct Upvalue {
    u8 index;
    bool is_local;
};

struct InternalCompiler;

struct InternalCompiler {
    InternalCompiler* enclosing;
    ObjFunction* function;
    FunctionType type;

    Local locals[256];
    i32 local_count;
    Upvalue upvalues[256];
    i32 scope_depth;
};

struct ClassCompiler;

struct ClassCompiler {
    ClassCompiler* enclosing;
    bool has_superclass;
};

Parser parser;
InternalCompiler* current = nullptr;
ClassCompiler* current_class = nullptr;

static inline Chunk& current_chunk() {
    return current->function->chunk;
}

static void error_at(const Token& token, const char* message) {
    if (parser.panic_mode)
        return;
    parser.panic_mode = true;

    eprint("[line {}] Error", token.line);

    switch (token.type) {
    case TokenType::Eof:
        eprint(" at end");
        break;
    case TokenType::Error:
        break;
    default: {
        const auto token_view = std::string_view { token.start, static_cast<std::string_view::size_type>(token.length) };
        eprint(" at '{}'", token_view);
    }
    }

    eprintln(": {}", message);
    parser.had_error = true;
}

static void error(const char* message) {
    error_at(parser.previous, message);
}

static void error_at_current(const char* message) {
    error_at(parser.current, message);
}

static inline void advance() {
    parser.previous = parser.current;

    while (true) {
        parser.current = scan_token();
        if (parser.current.type != TokenType::Error)
            break;

        error_at_current(parser.current.start);
    }
}

static inline void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    error_at_current(message);
}

static inline bool check(TokenType type) {
    return parser.current.type == type;
}

static inline bool match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

template <typename T>
concept Emittable = std::is_same_v<T, OpCode> || std::is_convertible_v<T, u8>;

static inline void emit(Emittable auto data) {
    current_chunk().write(static_cast<u8>(data), parser.previous.line);
}

static inline void emit(auto first, auto second) {
    emit(first);
    emit(second);
}

// This gets really stupid really quick. Luckily for us, there's no use in grouping more than three at a time.
static inline void emit(auto first, auto second, auto third) {
    emit(first);
    emit(second);
    emit(third);
}

static void emit_loop(i32 loop_start) {
    emit(OpCode::Loop);

    const auto offset = current_chunk().count - loop_start + 2;
    if (offset > std::numeric_limits<u16>::max())
        error("Loop body too large.");

    emit((offset >> 8) & 0xFF, offset & 0xFF);
}

static i32 emit_jump(OpCode op) {
    emit(op, 0xFF, 0xFF);
    return current_chunk().count - 2;
}

static void emit_return() {
    if (current->type == FunctionType::Initializer) {
        emit(OpCode::GetLocal, 0);
    } else {
        emit(OpCode::Nil);
    }

    emit(OpCode::Return);
}

static u8 make_constant(Value value) {
    const auto constant = current_chunk().add_constant(value);
    if (constant > std::numeric_limits<u8>::max()) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return static_cast<u8>(constant);
}

static void emit_constant(Value value) {
    emit(OpCode::Constant, make_constant(value));
}

static void patch_jump(i32 offset) {
    // -2 to adjust for the bytecode for the jump offset itself.
    const auto jump = current_chunk().count - offset - 2;
    if (jump > std::numeric_limits<u16>::max())
        error("Too much code to jump over.");

    current_chunk().code[offset] = static_cast<u8>((jump >> 8) & 0xff);
    current_chunk().code[offset + 1] = static_cast<u8>(jump & 0xff);
}

static void init_compiler(InternalCompiler& compiler, FunctionType type) {
    compiler.enclosing = reinterpret_cast<struct InternalCompiler*>(current);
    compiler.function = nullptr;
    compiler.type = type;
    compiler.local_count = 0;
    compiler.scope_depth = 0;
    // To state the book:
    // "I know, it looks dumb to null the function field only to immediately assign it a value a few lines later. More garbage collection-related paranoia."
    compiler.function = new_function();
    current = &compiler;
    if (type != FunctionType::Script)
        current->function->name = copy_string(parser.previous.start, parser.previous.length);

    auto& local = current->locals[current->local_count++];
    local.depth = 0;
    local.is_captured = false;
    if (type == FunctionType::Function) {
        local.name.start = "";
        local.name.length = 0;
    } else {
        local.name.start = "this";
        local.name.length = 4;
    }
}

static ObjFunction* end_compiler() {
    emit_return();
    auto* function = current->function;

    if constexpr (DEBUG_PRINT_CODE) {
        if (!parser.had_error)
            disassemble_chunk(&current_chunk(), function->name ? function->name->chars : "<script>");
    }

    current = current->enclosing;
    return function;
}

static void begin_scope() {
    current->scope_depth++;
}

static void end_scope() {
    current->scope_depth--;

    while (current->local_count > 0 && current->locals[current->local_count - 1].depth > current->scope_depth) {
        if (current->locals[current->local_count - 1].is_captured)
            emit(OpCode::CloseUpvalue);
        else
            emit(OpCode::Pop);
        current->local_count--;
    }
}

static void expression();
static void statement();
static void declaration();
static inline const ParseRule& get_rule(TokenType);
static void parse_precedence(Precedence);
static inline u8 identifier_constant(const Token&);
static i32 resolve_local(const InternalCompiler&, const Token&);
static i32 resolve_upvalue(InternalCompiler&, const Token&);
static u8 argument_list();
static void and_(bool);

static void binary([[maybe_unused]] bool) {
    const auto operator_type = parser.previous.type;
    const auto& rule = get_rule(operator_type);
    parse_precedence(static_cast<Precedence>(static_cast<i32>(rule.precedence) + 1));

    switch (operator_type) {
    case TokenType::BangEqual:
        emit(OpCode::Equal, OpCode::Not);
        break;
    case TokenType::EqualEqual:
        emit(OpCode::Equal);
        break;
    case TokenType::Greater:
        emit(OpCode::Greater);
        break;
    case TokenType::GreaterEqual:
        emit(OpCode::Less, OpCode::Not);
        break;
    case TokenType::Less:
        emit(OpCode::Less);
        break;
    case TokenType::LessEqual:
        emit(OpCode::Greater, OpCode::Not);
        break;
    case TokenType::Plus:
        emit(OpCode::Add);
        break;
    case TokenType::Minus:
        emit(OpCode::Subtract);
        break;
    case TokenType::Star:
        emit(OpCode::Multiply);
        break;
    case TokenType::Slash:
        emit(OpCode::Divide);
        break;
    default:
        VERIFY(false, "Unreachable code");
    }
}

static void call([[maybe_unused]] bool) {
    const auto arg_count = argument_list();
    emit(OpCode::Call, arg_count);
}

static void dot(bool can_assign) {
    consume(TokenType::Identifier, "Expect property name after '.'.");
    const auto name = identifier_constant(parser.previous);

    if (can_assign && match(TokenType::Equal)) {
        expression();
        emit(OpCode::SetProperty, name);
    } else if (match(TokenType::LeftParen)) {
        const auto arg_count = argument_list();
        emit(OpCode::Invoke, name, arg_count);
    } else {
        emit(OpCode::GetProperty, name);
    }
}

static void literal([[maybe_unused]] bool) {
    switch (parser.previous.type) {
    case TokenType::False:
        emit(OpCode::False);
        break;
    case TokenType::Nil:
        emit(OpCode::Nil);
        break;
    case TokenType::True:
        emit(OpCode::True);
        break;
    default:
        VERIFY(false, "Unreachable code");
    }
}

static void grouping([[maybe_unused]] bool) {
    expression();
    consume(TokenType::RightParen, "Expect ')' after expression.");
}

static void number([[maybe_unused]] bool) {
    const auto value = std::strtod(parser.previous.start, nullptr);
    emit_constant(NUMBER_VAL(value));
}

static void or_([[maybe_unused]] bool) {
    const auto else_jump = emit_jump(OpCode::JumpIfFalse);
    const auto end_jump = emit_jump(OpCode::Jump);

    patch_jump(else_jump);
    emit(OpCode::Pop);

    parse_precedence(Precedence::Or);
    patch_jump(end_jump);
}

static void string([[maybe_unused]] bool) {
    emit_constant(OBJ_VAL(copy_string(parser.previous.start + 1, parser.previous.length - 2)));
}

static void named_variable(Token name, bool can_assign) {
    OpCode get_op, set_op;
    auto arg = resolve_local(*current, name);
    if (arg != -1) {
        get_op = OpCode::GetLocal;
        set_op = OpCode::SetLocal;
    } else if ((arg = resolve_upvalue(*current, name)) != -1) {
        get_op = OpCode::GetUpvalue;
        set_op = OpCode::SetUpvalue;
    } else {
        arg = identifier_constant(name);
        get_op = OpCode::GetGlobal;
        set_op = OpCode::SetGlobal;
    }

    if (can_assign && match(TokenType::Equal)) {
        expression();
        emit(set_op, arg);
    } else {
        emit(get_op, arg);
    }
}

static void variable(bool can_assign) {
    named_variable(parser.previous, can_assign);
}

static inline Token synthetic_token(const char* text) {
    Token token {};
    token.start = text;
    token.length = static_cast<i32>(std::strlen(text));
    return token;
}

static void super_([[maybe_unused]] bool) {
    if (!current_class)
        error("Can't use 'super' outside of a class.");
    else if (!current_class->has_superclass)
        error("Can't use 'super' in a class with no superclass.");

    consume(TokenType::Dot, "Expect '.' after 'super'.");
    consume(TokenType::Identifier, "Expect superclass method name.");
    const auto name = identifier_constant(parser.previous);

    named_variable(synthetic_token("this"), false);

    if (match(TokenType::LeftParen)) {
        const auto arg_count = argument_list();
        named_variable(synthetic_token("super"), false);
        emit(OpCode::SuperInvoke, name, arg_count);
    } else {
        named_variable(synthetic_token("super"), false);
        emit(OpCode::GetSuper, name);
    }
}

static void this_([[maybe_unused]] bool) {
    if (!current_class) {
        error("Can't use 'this' outside of a class.");
        return;
    }

    variable(false);
}

static void unary([[maybe_unused]] bool) {
    const auto operator_type = parser.previous.type;

    // Compile the operand.
    parse_precedence(Precedence::Unary);

    // Emit the operator instruction.
    switch (operator_type) {
    case TokenType::Bang:
        emit(OpCode::Not);
        break;
    case TokenType::Minus:
        emit(OpCode::Negate);
        break;
    default:
        VERIFY(false, "Unreachable code");
    }
}

static void import([[maybe_unused]] bool) {
    consume(TokenType::Identifier, "Expect module name after 'import'.");
    const auto name = identifier_constant(parser.previous);
    println("import module: {}", AS_CSTRING(current_chunk().constants.values[name]));
}

// clang-format off
// ´std::unordered_map´s cannot be constexpr, could be better to replace this map
// with a static constexpr std::array<ParseRule> instead
static std::unordered_map<TokenType, ParseRule> rules_map = {
    { TokenType::LeftParen,    { grouping,    call, Precedence::Call       } },
    { TokenType::RightParen,   {  nullptr, nullptr, Precedence::None       } },
    { TokenType::LeftBrace,    {  nullptr, nullptr, Precedence::None       } },
    { TokenType::RightBrace,   {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Comma,        {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Dot,          {  nullptr,     dot, Precedence::Call       } },
    { TokenType::Minus,        {    unary,  binary, Precedence::Term       } },
    { TokenType::Plus,         {  nullptr,  binary, Precedence::Term       } },
    { TokenType::Semicolon,    {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Slash,        {  nullptr,  binary, Precedence::Factor     } },
    { TokenType::Star,         {  nullptr,  binary, Precedence::Factor     } },
    { TokenType::Bang,         {  unary,   nullptr, Precedence::None       } },
    { TokenType::BangEqual,    {  nullptr,  binary, Precedence::Equality   } },
    { TokenType::Equal,        {  nullptr, nullptr, Precedence::None       } },
    { TokenType::EqualEqual,   {  nullptr,  binary, Precedence::Equality   } },
    { TokenType::Greater,      {  nullptr,  binary, Precedence::Comparison } },
    { TokenType::GreaterEqual, {  nullptr,  binary, Precedence::Comparison } },
    { TokenType::Less,         {  nullptr,  binary, Precedence::Comparison } },
    { TokenType::LessEqual,    {  nullptr,  binary, Precedence::Comparison } },
    { TokenType::Identifier,   { variable, nullptr, Precedence::None       } },
    { TokenType::String,       {   string, nullptr, Precedence::None       } },
    { TokenType::Number,       {   number, nullptr, Precedence::None       } },
    { TokenType::And,          {  nullptr,    and_, Precedence::And        } },
    { TokenType::Class,        {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Else,         {  nullptr, nullptr, Precedence::None       } },
    { TokenType::False,        {  literal, nullptr, Precedence::None       } },
    { TokenType::For,          {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Fun,          {  nullptr, nullptr, Precedence::None       } },
    { TokenType::If,           {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Import,       {   import, nullptr, Precedence::None       } },
    { TokenType::Nil,          {  literal, nullptr, Precedence::None       } },
    { TokenType::Or,           {  nullptr,     or_, Precedence::Or         } },
    { TokenType::Print,        {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Return,       {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Super,        {   super_, nullptr, Precedence::None       } },
    { TokenType::This,         {    this_, nullptr, Precedence::None       } },
    { TokenType::True,         {  literal, nullptr, Precedence::None       } },
    { TokenType::Var,          {  nullptr, nullptr, Precedence::None       } },
    { TokenType::While,        {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Error,        {  nullptr, nullptr, Precedence::None       } },
    { TokenType::Eof,          {  nullptr, nullptr, Precedence::None       } },
};
// clang-format on

static void parse_precedence(Precedence precedence) {
    advance();
    const auto prefix_rule = get_rule(parser.previous.type).prefix;
    if (!prefix_rule) {
        error("Expect expression.");
        return;
    }

    const auto can_assign = precedence <= Precedence::Assignment;
    prefix_rule(can_assign);

    while (precedence <= get_rule(parser.current.type).precedence) {
        advance();
        const auto infix_rule = get_rule(parser.previous.type).infix;
        infix_rule(can_assign);
    }

    if (can_assign && match(TokenType::Equal))
        error("Invalid assignment target.");
}

static inline u8 identifier_constant(const Token& name) {
    return make_constant(OBJ_VAL(copy_string(name.start, name.length)));
}

static inline bool identifiers_equal(const Token& a, const Token& b) {
    if (a.length != b.length)
        return false;
    return std::memcmp(a.start, b.start, static_cast<size_t>(a.length)) == 0;
}

static i32 resolve_local(const InternalCompiler& compiler, const Token& name) {
    for (auto i = compiler.local_count - 1; i >= 0; i--) {
        const auto& local = compiler.locals[i];
        if (identifiers_equal(name, local.name)) {
            if (local.depth == -1)
                error("Can't read local variable in its own initializer.");

            return i;
        }
    }

    return -1;
}

static i32 add_upvalue(InternalCompiler& compiler, u8 index, bool is_local) {
    const auto upvalue_count = compiler.function->upvalue_count;

    for (auto i = 0; i < upvalue_count; i++) {
        const auto& upvalue = compiler.upvalues[i];
        if (upvalue.index == index && upvalue.is_local == is_local)
            return i;
    }

    if (upvalue_count == std::numeric_limits<u8>::max() + 1) [[unlikely]] {
        error("Too many closure variables in function.");
        return 0;
    }

    compiler.upvalues[upvalue_count].is_local = is_local;
    compiler.upvalues[upvalue_count].index = index;
    return compiler.function->upvalue_count++;
}

static i32 resolve_upvalue(InternalCompiler& compiler, const Token& name) {
    if (!compiler.enclosing)
        return -1;

    const auto local = resolve_local(*compiler.enclosing, name);
    if (local != -1) {
        compiler.enclosing->locals[local].is_captured = true;
        return add_upvalue(compiler, static_cast<u8>(local), true);
    }

    const auto upvalue = resolve_upvalue(*compiler.enclosing, name);
    if (upvalue != -1)
        return add_upvalue(compiler, static_cast<u8>(upvalue), false);

    return -1;
}

static void add_local(Token name) {
    if (current->local_count == std::numeric_limits<u8>::max() + 1) {
        error("Too many local variables in function.");
        return;
    }

    auto* local = &current->locals[current->local_count++];
    local->name = name;
    local->depth = -1;
    local->is_captured = false;
}

static void declare_variable() {
    if (current->scope_depth == 0)
        return;

    auto& name = parser.previous;

    for (auto i = current->local_count - 1; i >= 0; i--) {
        auto* local = &current->locals[i];
        if (local->depth != -1 && local->depth < current->scope_depth)
            break;

        if (identifiers_equal(name, local->name))
            error("Already a variable with this name in this scope.");
    }

    add_local(name);
}

static u8 parse_variable(const char* error_message) {
    consume(TokenType::Identifier, error_message);

    declare_variable();
    if (current->scope_depth > 0)
        return 0;

    return identifier_constant(parser.previous);
}

static inline void mark_initialized() {
    if (current->scope_depth == 0)
        return;

    current->locals[current->local_count - 1].depth = current->scope_depth;
}

static inline void define_variable(u8 global) {
    if (current->scope_depth > 0) {
        mark_initialized();
        return;
    }
    emit(OpCode::DefineGlobal, global);
}

static u8 argument_list() {
    u8 arg_count = 0;
    if (!check(TokenType::RightParen)) {
        do {
            expression();
            if (arg_count++ == std::numeric_limits<u8>::max())
                error("Can't have more than 255 arguments.");
        } while (match(TokenType::Comma));
    }
    consume(TokenType::RightParen, "Expect ')' after arguments.");
    return arg_count;
}

static void and_([[maybe_unused]] bool) {
    const auto end_jump = emit_jump(OpCode::JumpIfFalse);

    emit(OpCode::Pop);
    parse_precedence(Precedence::And);
    patch_jump(end_jump);
}

static inline const ParseRule& get_rule(TokenType type) {
    return rules_map[type];
}

static void expression() {
    // Compile the operand.
    parse_precedence(Precedence::Assignment);
}

static inline void block() {
    while (!check(TokenType::RightBrace) && !check(TokenType::Eof)) {
        declaration();
    }

    consume(TokenType::RightBrace, "Expect '}' after block.");
}

static void function(FunctionType type) {
    InternalCompiler compiler {};
    init_compiler(compiler, type);
    begin_scope();

    consume(TokenType::LeftParen, "Expect '(' after function name.");
    if (!check(TokenType::RightParen)) {
        do {
            if (++(current->function->arity) > std::numeric_limits<u8>::max())
                error_at_current("Can't have more than 255 parameters.");
            const auto constant = parse_variable("Expect parameter name.");
            define_variable(constant);
        } while (match(TokenType::Comma));
    }
    consume(TokenType::RightParen, "Expect ')' after parameters.");
    consume(TokenType::LeftBrace, "Expect '{' before function body.");
    block();

    auto* function = end_compiler();
    emit(OpCode::Closure, make_constant(OBJ_VAL(function)));

    for (auto i = 0; i < function->upvalue_count; i++) {
        auto upvalue = compiler.upvalues[i];
        emit(upvalue.is_local, upvalue.index);
    }
}

static void method() {
    consume(TokenType::Identifier, "Expect method name.");
    const auto constant = identifier_constant(parser.previous);

    auto type = FunctionType::Method;
    if (parser.previous.length == 4 && std::memcmp(parser.previous.start, "init", 4) == 0) {
        type = FunctionType::Initializer;
    }
    function(type);
    emit(OpCode::Method, constant);
}

static void class_declaration() {
    consume(TokenType::Identifier, "Expect class name.");
    auto class_name = parser.previous;
    const auto name_constant = identifier_constant(parser.previous);
    declare_variable();

    emit(OpCode::Class, name_constant);
    define_variable(name_constant);

    ClassCompiler class_compiler {};
    class_compiler.has_superclass = false;
    class_compiler.enclosing = current_class;
    current_class = &class_compiler;

    if (match(TokenType::Colon)) {
        consume(TokenType::Identifier, "Expect superclass name.");
        variable(false);

        if (identifiers_equal(class_name, parser.previous))
            error("A class can't inherit from itself.");

        begin_scope();
        add_local(synthetic_token("super"));
        define_variable(0);

        named_variable(class_name, false);
        emit(OpCode::Inherit);
        class_compiler.has_superclass = true;
    }

    named_variable(class_name, false);

    consume(TokenType::LeftBrace, "Expect '{' before class body.");
    while (!check(TokenType::RightBrace) && !check(TokenType::Eof)) {
        method();
    }
    consume(TokenType::RightBrace, "Expect '}' after class body.");
    emit(OpCode::Pop);

    if (class_compiler.has_superclass)
        end_scope();

    current_class = current_class->enclosing;
}

static void fun_declaration() {
    const auto global = parse_variable("Expect function name.");
    mark_initialized();
    function(FunctionType::Function);
    define_variable(global);
}

static void var_declaration() {
    const auto global = parse_variable("Expect variable name.");

    if (match(TokenType::Equal))
        expression();
    else
        emit(OpCode::Nil);
    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");

    define_variable(global);
}

static void print_statement() {
    expression();
    consume(TokenType::Semicolon, "Expect ';' after value.");
    emit(OpCode::Print);
}

static void return_statement() {
    if (current->type == FunctionType::Script)
        error("Can't return from top-level code.");

    if (match(TokenType::Semicolon)) {
        emit_return();
    } else {
        if (current->type == FunctionType::Initializer)
            error("Can't return a value from an initializer.");

        expression();
        consume(TokenType::Semicolon, "Expect ';' after return value.");
        emit(OpCode::Return);
    }
}

static void while_statement() {
    const auto loop_start = current_chunk().count;
    consume(TokenType::LeftParen, "Expect '(' after 'while'.");
    expression();
    consume(TokenType::RightParen, "Expect ')' after condition.");

    const auto exit_jump = emit_jump(OpCode::JumpIfFalse);
    emit(OpCode::Pop);
    statement();
    emit_loop(loop_start);
    patch_jump(exit_jump);
    emit(OpCode::Pop);
}

static void synchronize() {
    parser.panic_mode = false;

    while (parser.current.type != TokenType::Eof) {
        if (parser.previous.type == TokenType::Semicolon)
            return;
        switch (parser.current.type) {
        case TokenType::Class:
        case TokenType::Fun:
        case TokenType::Var:
        case TokenType::For:
        case TokenType::If:
        case TokenType::While:
        case TokenType::Print:
        case TokenType::Return:
            return;

        default:; // Do nothing.
        }

        advance();
    }
}

static void expression_statement() {
    expression();
    consume(TokenType::Semicolon, "Expect ';' after expression.");
    emit(OpCode::Pop);
}

static void for_statement() {
    begin_scope();
    consume(TokenType::LeftParen, "Expect '(' after 'for'.");

    if (match(TokenType::Semicolon)) {
        // No initializer.
    } else if (match(TokenType::Var)) {
        var_declaration();
    } else {
        expression_statement();
    }

    auto loop_start = current_chunk().count;
    auto exit_jump = -1;
    if (!match(TokenType::Semicolon)) {
        expression();
        consume(TokenType::Semicolon, "Expect ';' after loop condition.");

        // Jump out of the loop if the condition is false.
        exit_jump = emit_jump(OpCode::JumpIfFalse);
        emit(OpCode::Pop); // Condition.
    }

    if (!match(TokenType::RightParen)) {
        const auto body_jump = emit_jump(OpCode::Jump);
        const auto increment_start = current_chunk().count;
        expression();
        emit(OpCode::Pop);
        consume(TokenType::RightParen, "Expect ')' after for clauses.");

        emit_loop(loop_start);
        loop_start = increment_start;
        patch_jump(body_jump);
    }

    statement();
    emit_loop(loop_start);

    if (exit_jump != -1) {
        patch_jump(exit_jump);
        emit(OpCode::Pop); // Condition.
    }
    end_scope();
}

static void if_statement() {
    consume(TokenType::LeftParen, "Expect '(' after 'if'.");
    expression();
    consume(TokenType::RightParen, "Expect ')' after condition.");

    const auto then_jump = emit_jump(OpCode::JumpIfFalse);
    emit(OpCode::Pop);
    statement();

    const auto else_jump = emit_jump(OpCode::Jump);

    patch_jump(then_jump);
    emit(OpCode::Pop);

    if (match(TokenType::Else))
        statement();
    patch_jump(else_jump);
}

static void declaration() {
    if (match(TokenType::Class)) {
        class_declaration();
    } else if (match(TokenType::Fun)) {
        fun_declaration();
    } else if (match(TokenType::Var)) {
        var_declaration();
    } else {
        statement();
    }

    if (parser.panic_mode)
        synchronize();
}

static void statement() {
    if (match(TokenType::Print)) {
        print_statement();
    } else if (match(TokenType::LeftBrace)) {
        begin_scope();
        block();
        end_scope();
    } else if (match(TokenType::If)) {
        if_statement();
    } else if (match(TokenType::Return)) {
        return_statement();
    } else if (match(TokenType::While)) {
        while_statement();
    } else if (match(TokenType::For)) {
        for_statement();
    } else {
        expression_statement();
    }
}

ObjFunction* compile(const char* source) {
    init_scanner(source);

    auto compiler = InternalCompiler {};
    init_compiler(compiler, FunctionType::Script);

    parser.had_error = false;
    parser.panic_mode = false;

    advance();
    while (!match(TokenType::Eof)) {
        declaration();
    }
    auto* function = end_compiler();
    return parser.had_error ? nullptr : function;
}

void mark_compiler_roots() {
    auto* compiler = current;
    while (compiler) {
        mark_object(reinterpret_cast<Obj*>(compiler->function));
        compiler = compiler->enclosing;
    }
}

ObjFunction* Compiler::compile(std::string_view source) {
    return calamus::script::compile(source.data());
}

}
