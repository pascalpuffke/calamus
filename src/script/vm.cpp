#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <script/compiler.h>
#include <script/config.h>
#include <script/debug.h>
#include <script/memory.h>
#include <script/vm.h>
#include <util/common.h>
#include <util/scoped_timer.h>

namespace calamus::script::VM {

#define READ_BYTE() (*frame->ip++)
#define READ_SHORT() \
    (frame->ip += 2, static_cast<u16>((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() \
    (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(value_type, op)                         \
    do {                                                  \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtime_error("Operands must be numbers.");   \
            return InterpretResult::RuntimeError;         \
        }                                                 \
        f64 b = AS_NUMBER(pop());                         \
        f64 a = AS_NUMBER(pop());                         \
        push(value_type(a op b));                         \
    } while (0)

VM vm;

static Value clock_native([[maybe_unused]] i32, [[maybe_unused]] Value*) {
    return NUMBER_VAL(static_cast<f64>(clock()) / CLOCKS_PER_SEC);
}

static Value memory_native([[maybe_unused]] i32, [[maybe_unused]] Value*) {
    return NUMBER_VAL(static_cast<f64>(vm.bytes_allocated));
}

static Value gc_native([[maybe_unused]] i32, [[maybe_unused]] Value*) {
    gc();
    return NIL_VAL;
}

static void reset_stack() {
    vm.stack_top = vm.stack;
    vm.frame_count = 0;
    vm.open_upvalues = nullptr;
}

static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    for (auto i = vm.frame_count - 1; i >= 0; i--) {
        const auto* frame = &vm.frames[i];
        const auto* function = frame->closure->function;
        const auto instruction = frame->ip - function->chunk.code - 1;
        eprint("[line {}] in ", function->chunk.lines[instruction]);
        if (function->name)
            eprintln("{}()", function->name->chars);
        else
            eprintln("script");
    }

    reset_stack();
}

static inline Value peek(i32 distance) {
    return vm.stack_top[-1 - distance];
}

static bool call(ObjClosure* closure, i32 arg_count) {
    if (arg_count != closure->function->arity) [[unlikely]] {
        runtime_error("Expected %d arguments but got %d.", closure->function->arity, arg_count);
        return false;
    }

    if (vm.frame_count == FRAMES_MAX) [[unlikely]] {
        runtime_error("Stack overflow.");
        return false;
    }

    auto* frame = &vm.frames[vm.frame_count++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
    frame->slots = vm.stack_top - arg_count - 1;
    return true;
}

static bool call_value(Value callee, i32 arg_count) {
    if (IS_OBJ(callee)) {
        switch (OBJ_TYPE(callee)) {
        case ObjType::BoundMethod: {
            auto* bound = AS_BOUND_METHOD(callee);
            vm.stack_top[-arg_count - 1] = bound->receiver;
            return call(bound->method, arg_count);
        }
        case ObjType::Class: {
            auto* klass = AS_CLASS(callee);
            vm.stack_top[-arg_count - 1] = OBJ_VAL(new_instance(klass));

            // Run class constructor if one exists
            if (auto initializer = klass->methods.get(vm.init_string))
                return call(AS_CLOSURE(initializer.value()), arg_count);

            if (arg_count != 0) {
                runtime_error("Expected 0 arguments but got %d.", arg_count);
                return false;
            }
            return true;
        }
        case ObjType::Closure:
            return call(AS_CLOSURE(callee), arg_count);
        case ObjType::Native: {
            auto native = AS_NATIVE(callee);
            auto result = native(arg_count, vm.stack_top - arg_count);
            vm.stack_top -= arg_count + 1;
            push(result);
            return true;
        }
        default:
            break; // Non-callable object type.
        }
    }
    runtime_error("Can only call functions and classes.");
    return false;
}

static bool invoke_from_class(ObjClass* klass, ObjString* name, i32 arg_count) {
    auto method = klass->methods.get(name);
    if (!method.has_value()) [[unlikely]] {
        runtime_error("Undefined property '%s'.", name->chars);
        return false;
    }
    return call(AS_CLOSURE(method.value()), arg_count);
}

static bool invoke(ObjString* name, i32 arg_count) {
    const auto receiver = peek(arg_count);
    if (!IS_INSTANCE(receiver)) {
        runtime_error("Only instances have methods.");
        return false;
    }

    auto* instance = AS_INSTANCE(receiver);
    if (auto maybe_value = instance->fields.get(name)) {
        auto& value = maybe_value.value();
        vm.stack_top[-arg_count - 1] = value;
        return call_value(value, arg_count);
    }

    return invoke_from_class(instance->klass, name, arg_count);
}

static bool bind_method(ObjClass* klass, ObjString* name) {
    auto method = klass->methods.get(name);
    if (!method.has_value()) [[unlikely]] {
        runtime_error("Undefined property '%s'.", name->chars);
        return false;
    }

    auto* bound = new_bound_method(peek(0), AS_CLOSURE(method.value()));
    pop();
    push(OBJ_VAL(bound));
    return true;
}

static ObjUpvalue* capture_upvalue(Value* local) {
    ObjUpvalue* prev_upvalue = nullptr;
    auto* upvalue = vm.open_upvalues;
    while (upvalue && upvalue->location > local) {
        prev_upvalue = upvalue;
        upvalue = upvalue->next;
    }

    if (upvalue && upvalue->location == local)
        return upvalue;
    auto* created_upvalue = new_upvalue(local);
    created_upvalue->next = upvalue;

    if (prev_upvalue)
        prev_upvalue->next = created_upvalue;
    else
        vm.open_upvalues = created_upvalue;

    return created_upvalue;
}

static void close_upvalues(const Value* last) {
    while (vm.open_upvalues && vm.open_upvalues->location >= last) {
        auto* upvalue = vm.open_upvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        vm.open_upvalues = upvalue->next;
    }
}

static inline void define_method(ObjString* name) {
    auto method = peek(0);
    auto* klass = AS_CLASS(peek(1));
    klass->methods.set(name, method);
    pop();
}

static inline bool is_falsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    auto* b = AS_STRING(peek(0));
    auto* a = AS_STRING(peek(1));

    const auto length = a->length + b->length;
    auto* chars = ALLOCATE(char, length + 1);
    std::memcpy(chars, a->chars, static_cast<usize>(a->length));
    std::memcpy(chars + a->length, b->chars, static_cast<usize>(b->length));
    chars[length] = '\0';

    auto* result = take_string(chars, length);
    pop();
    pop();
    push(OBJ_VAL(result));
}

void push(Value value) { *vm.stack_top++ = value; }
Value pop() { return *--vm.stack_top; }

static void define_native(const char* name, NativeFn function) {
    push(OBJ_VAL(copy_string(name, static_cast<i32>(strlen(name)))));
    push(OBJ_VAL(new_native(function)));
    vm.globals.set(AS_STRING(vm.stack[0]), vm.stack[1]);
    pop();
    pop();
}

static InterpretResult run() {
    auto* frame = &vm.frames[vm.frame_count - 1];

    while (true) {
        if constexpr (DEBUG_TRACE_EXECUTION) {
            fmt::print("          ");
            for (auto* slot = vm.stack; slot < vm.stack_top; slot++) {
                fmt::print("[ ");
                print_value(*slot);
                fmt::print(" ]");
            }
            println();
            disassemble_instruction(&frame->closure->function->chunk, static_cast<i32>(frame->ip - frame->closure->function->chunk.code));
        }
        switch (static_cast<OpCode>(READ_BYTE())) {
        case OpCode::Constant: {
            const auto constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OpCode::Nil:
            push(NIL_VAL);
            break;
        case OpCode::True:
            push(BOOL_VAL(true));
            break;
        case OpCode::False:
            push(BOOL_VAL(false));
            break;
        case OpCode::Pop:
            pop();
            break;
        case OpCode::GetLocal: {
            const auto slot = READ_BYTE();
            push(frame->slots[slot]);
            break;
        }
        case OpCode::SetLocal: {
            const auto slot = READ_BYTE();
            frame->slots[slot] = peek(0);
            break;
        }
        case OpCode::GetGlobal: {
            auto* name = READ_STRING();
            auto global = vm.globals.get(name);
            if (!global.has_value()) [[unlikely]] {
                runtime_error("Undefined variable '%s'.", name->chars);
                return InterpretResult::RuntimeError;
            }
            push(global.value());
            break;
        }
        case OpCode::DefineGlobal: {
            auto* name = READ_STRING();
            vm.globals.set(name, peek(0));
            pop();
            break;
        }
        case OpCode::SetGlobal: {
            auto* name = READ_STRING();
            if (vm.globals.set(name, peek(0))) [[unlikely]] {
                vm.globals.remove(name);
                runtime_error("Undefined variable '%s'.", name->chars);
                return InterpretResult::RuntimeError;
            }
            break;
        }
        case OpCode::GetUpvalue: {
            const auto slot = READ_BYTE();
            push(*frame->closure->upvalues[slot]->location);
            break;
        }
        case OpCode::SetUpvalue: {
            const auto slot = READ_BYTE();
            *frame->closure->upvalues[slot]->location = peek(0);
            break;
        }
        case OpCode::GetProperty: {
            if (!IS_INSTANCE(peek(0))) {
                runtime_error("Only instances have properties.");
                return InterpretResult::RuntimeError;
            }

            auto* instance = AS_INSTANCE(peek(0));
            auto* name = READ_STRING();

            if (auto property = instance->fields.get(name)) {
                pop(); // Instance.
                push(property.value());
                break;
            }

            if (!bind_method(instance->klass, name))
                return InterpretResult::RuntimeError;
            break;
        }
        case OpCode::SetProperty: {
            if (!IS_INSTANCE(peek(1))) {
                runtime_error("Only instances have fields.");
                return InterpretResult::RuntimeError;
            }

            auto* instance = AS_INSTANCE(peek(1));
            instance->fields.set(READ_STRING(), peek(0));
            const auto value = pop();
            pop();
            push(value);
            break;
        }
        case OpCode::GetSuper: {
            auto* name = READ_STRING();
            auto* superclass = AS_CLASS(pop());

            if (!bind_method(superclass, name))
                return InterpretResult::RuntimeError;
            break;
        }
        case OpCode::Equal: {
            const auto b = pop();
            const auto a = pop();
            push(BOOL_VAL(values_equal(a, b)));
            break;
        }
        case OpCode::Greater:
            BINARY_OP(BOOL_VAL, >);
            break;
        case OpCode::Less:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OpCode::Add:
            if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                concatenate();
            } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                const auto b = AS_NUMBER(pop());
                const auto a = AS_NUMBER(pop());
                push(NUMBER_VAL(a + b));
            } else {
                runtime_error("Operands must be two numbers or two strings.");
                return InterpretResult::RuntimeError;
            }
            break;
        case OpCode::Subtract:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OpCode::Multiply:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OpCode::Divide:
            BINARY_OP(NUMBER_VAL, /);
            break;
        case OpCode::Not:
            push(BOOL_VAL(is_falsey(pop())));
            break;
        case OpCode::Negate: {
            if (!IS_NUMBER(peek(0))) {
                runtime_error("Operand must be a number.");
                return InterpretResult::RuntimeError;
            }
            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;
        }
        case OpCode::Print: {
            print_value(pop());
            println();
            break;
        }
        case OpCode::Jump: {
            const auto offset = READ_SHORT();
            frame->ip += offset;
            break;
        }
        case OpCode::JumpIfFalse: {
            const auto offset = READ_SHORT();
            if (is_falsey(peek(0)))
                frame->ip += offset;
            break;
        }
        case OpCode::Loop: {
            const auto offset = READ_SHORT();
            frame->ip -= offset;
            break;
        }
        case OpCode::Call: {
            const auto arg_count = READ_BYTE();
            if (!call_value(peek(arg_count), arg_count))
                return InterpretResult::RuntimeError;
            frame = &vm.frames[vm.frame_count - 1];
            break;
        }
        case OpCode::Invoke: {
            auto* method = READ_STRING();
            const auto arg_count = READ_BYTE();
            if (!invoke(method, arg_count))
                return InterpretResult::RuntimeError;
            frame = &vm.frames[vm.frame_count - 1];
            break;
        }
        case OpCode::SuperInvoke: {
            auto* method = READ_STRING();
            const auto arg_count = READ_BYTE();
            auto* superclass = AS_CLASS(pop());
            if (!invoke_from_class(superclass, method, arg_count))
                return InterpretResult::RuntimeError;
            frame = &vm.frames[vm.frame_count - 1];
            break;
        }
        case OpCode::Closure: {
            auto* function = AS_FUNCTION(READ_CONSTANT());
            auto* closure = new_closure(function);
            push(OBJ_VAL(closure));

            for (auto i = 0; i < closure->upvalue_count; i++) {
                auto is_local = READ_BYTE();
                auto index = READ_BYTE();
                if (is_local)
                    closure->upvalues[i] = capture_upvalue(frame->slots + index);
                else
                    closure->upvalues[i] = frame->closure->upvalues[index];
            }

            break;
        }
        case OpCode::CloseUpvalue:
            close_upvalues(vm.stack_top - 1);
            pop();
            break;
        case OpCode::Return: {
            const auto result = pop();
            close_upvalues(frame->slots);
            vm.frame_count--;
            if (vm.frame_count == 0) {
                pop();
                return InterpretResult::Ok;
            }

            vm.stack_top = frame->slots;
            push(result);
            frame = &vm.frames[vm.frame_count - 1];
            break;
        }
        case OpCode::Class:
            push(OBJ_VAL(new_class(READ_STRING())));
            break;
        case OpCode::Inherit: {
            const auto superclass = peek(1);
            if (!IS_CLASS(superclass)) {
                runtime_error("Superclass must be a class.");
                return InterpretResult::RuntimeError;
            }

            auto* subclass = AS_CLASS(peek(0));
            auto* subclass_methods = &AS_CLASS(superclass)->methods;
            for (const auto& [key, value] : subclass_methods->entries())
                subclass->methods.set(key, value);

            pop(); // Subclass.
            break;
        }
        case OpCode::Method:
            define_method(READ_STRING());
            break;
        default:
            UNREACHABLE();
        }
    }
    UNREACHABLE();
}

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_SHORT
#undef READ_STRING
#undef BINARY_OP

void init_vm() {
    reset_stack();
    vm.objects = nullptr;
    vm.bytes_allocated = 0;
    vm.next_gc = 1024 * 1024;

    vm.gray_count = 0;
    vm.gray_capacity = 0;
    vm.gray_stack = nullptr;

    vm.globals.reset();
    vm.strings.reset();

    vm.init_string = nullptr;
    vm.init_string = copy_string("init", 4);

    define_native("clock", clock_native);
    define_native("memory", memory_native);
    define_native("gc", gc_native);
}

void free_vm() {
    vm.globals.free();
    vm.strings.free();
    vm.init_string = nullptr;
    free_objects();
}

InterpretResult interpret(std::string_view source) {
    ObjFunction* function;
    if constexpr (DEBUG_MEASURE_PERF) {
        const auto timer = ScopedTimer { "compile" };
        auto compiler = Compiler();
        function = compiler.compile(source);
    } else {
        auto compiler = Compiler();
        function = compiler.compile(source);
    }

    if (!function)
        return InterpretResult::CompileError;

    if constexpr (DEBUG_MEASURE_PERF) {
        const auto timer = ScopedTimer { "vm" };

        push(OBJ_VAL(function));

        auto* closure = new_closure(function);
        pop();
        push(OBJ_VAL(closure));
        call(closure, 0);

        return run();
    } else {
        push(OBJ_VAL(function));

        auto* closure = new_closure(function);
        pop();
        push(OBJ_VAL(closure));
        call(closure, 0);

        return run();
    }
}

}
