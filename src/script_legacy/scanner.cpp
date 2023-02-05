#include <cctype>
#include <cstring>
#include <script_legacy/scanner.h>
#include <util/assert.h>
#include <util/types.h>

namespace calamus::script {

struct Scanner {
    const char* start;
    const char* current;
    i32 line;
};

Scanner scanner;

static inline bool is_at_end() { return *scanner.current == '\0'; }

static inline char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static inline bool match(char expected) {
    if (is_at_end())
        return false;
    if (*scanner.current != expected)
        return false;
    scanner.current++;
    return true;
}

static inline char peek() { return *scanner.current; }

static inline char peek_next() {
    if (is_at_end())
        return '\0';
    return scanner.current[1];
}

static void skip_whitespace() {
    while (true) {
        char c = peek();
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            scanner.line++;
            advance();
            break;
        case '/':
            if (peek_next() == '/') {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !is_at_end())
                    advance();
            } else
                return;
            break;
        default:
            return;
        }
    }
}

static Token make_token(TokenType type) {
    Token token {};
    token.type = type;
    token.start = scanner.start;
    token.length = static_cast<i32>(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token error_token(const char* message) {
    Token token {};
    token.type = TokenType::Error;
    token.start = message;
    token.length = static_cast<i32>(strlen(message));
    token.line = scanner.line;
    return token;
}

static Token string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n')
            scanner.line++;
        advance();
    }

    if (is_at_end())
        return error_token("Unterminated string.");

    // The closing quote.
    advance();
    return make_token(TokenType::String);
}

static TokenType check_keyword(i32 start, i32 length, const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0)
        return type;

    return TokenType::Identifier;
}

static TokenType identifier_type() {
    switch (scanner.start[0]) {
    case 'a':
        return check_keyword(1, 2, "nd", TokenType::And);
    case 'c':
        return check_keyword(1, 4, "lass", TokenType::Class);
    case 'e':
        return check_keyword(1, 3, "lse", TokenType::Else);
    case 'f':
        if (scanner.current - scanner.start > 1) {
            switch (scanner.start[1]) {
            case 'a':
                return check_keyword(2, 3, "lse", TokenType::False);
            case 'o':
                return check_keyword(2, 1, "r", TokenType::For);
            case 'u':
                return check_keyword(2, 1, "n", TokenType::Fun);
            }
        }
        break;
    case 'i':
        if (scanner.current - scanner.start > 1) {
            switch (scanner.start[1]) {
            case 'f':
                return TokenType::If;
            case 'm':
                return check_keyword(2, 4, "port", TokenType::Import);
            }
        }
        break;
    case 'n':
        return check_keyword(1, 2, "il", TokenType::Nil);
    case 'o':
        return check_keyword(1, 1, "r", TokenType::Or);
    case 'p':
        return check_keyword(1, 4, "rint", TokenType::Print);
    case 'r':
        return check_keyword(1, 5, "eturn", TokenType::Return);
    case 's':
        return check_keyword(1, 4, "uper", TokenType::Super);
    case 't':
        if (scanner.current - scanner.start > 1) {
            switch (scanner.start[1]) {
            case 'h':
                return check_keyword(2, 2, "is", TokenType::This);
            case 'r':
                return check_keyword(2, 2, "ue", TokenType::True);
            }
        }
        break;
    case 'v':
        return check_keyword(1, 2, "ar", TokenType::Var);
    case 'w':
        return check_keyword(1, 4, "hile", TokenType::While);
    }
    return TokenType::Identifier;
}

static Token identifier() {
    while (isalnum(peek()) || peek() == '_')
        advance();
    return make_token(identifier_type());
}

static Token number() {
    while (isdigit(peek()))
        advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peek_next())) {
        // Consume the ".".
        advance();

        while (isdigit(peek()))
            advance();
    }

    return make_token(TokenType::Number);
}

void init_scanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

Token scan_token() {
    skip_whitespace();
    scanner.start = scanner.current;

    if (is_at_end())
        return make_token(TokenType::Eof);

    char c = advance();
    if (isalpha(c))
        return identifier();
    if (isdigit(c))
        return number();

    switch (c) {
    case '(':
        return make_token(TokenType::LeftParen);
    case ')':
        return make_token(TokenType::RightParen);
    case '{':
        return make_token(TokenType::LeftBrace);
    case '}':
        return make_token(TokenType::RightBrace);
    case ';':
        return make_token(TokenType::Semicolon);
    case ':':
        return make_token(TokenType::Colon);
    case ',':
        return make_token(TokenType::Comma);
    case '.':
        return make_token(TokenType::Dot);
    case '-':
        return make_token(TokenType::Minus);
    case '+':
        return make_token(TokenType::Plus);
    case '/':
        return make_token(TokenType::Slash);
    case '*':
        return make_token(TokenType::Star);
    case '!':
        return make_token(
            match('=') ? TokenType::BangEqual : TokenType::Bang);
    case '=':
        return make_token(
            match('=') ? TokenType::EqualEqual : TokenType::Equal);
    case '<':
        return make_token(
            match('=') ? TokenType::LessEqual : TokenType::Less);
    case '>':
        return make_token(
            match('=') ? TokenType::GreaterEqual : TokenType::Greater);
    case '"':
        return string();
    default:
        return error_token("Unexpected character.");
    }
    UNREACHABLE();
}

}
