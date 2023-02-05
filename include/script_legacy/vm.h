#pragma once

#include <script_legacy/object.h>
#include <script_legacy/table.h>
#include <script_legacy/value.h>
#include <string_view>

constexpr static auto FRAMES_MAX = 64;
constexpr static auto STACK_MAX = (FRAMES_MAX * 256);

namespace calamus::script::VM {

struct CallFrame {
    ObjClosure* closure;
    u8* ip;
    Value* slots;
};

struct VM {
    CallFrame frames[FRAMES_MAX] {};
    i32 frame_count {};

    Value stack[STACK_MAX] {};
    Value* stack_top {};
    Table globals;
    Table strings;
    ObjString* init_string {};
    ObjUpvalue* open_upvalues {};

    size_t bytes_allocated {};
    size_t next_gc {};
    Obj* objects {};

    i32 gray_count {};
    i32 gray_capacity {};
    Obj** gray_stack {};
};

enum class InterpretResult {
    Ok,
    CompileError,
    RuntimeError,
};

extern VM vm;

void init_vm();
void free_vm();

InterpretResult interpret(std::string_view);

void push(Value);
Value pop();

}
