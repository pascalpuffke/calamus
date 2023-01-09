#pragma once

#include <script/config.h>
#include <script/value.h>

namespace calamus::script {

enum class OpCode : u8 {
    Constant,
    Nil,
    True,
    False,
    Pop,
    GetLocal,
    SetLocal,
    GetGlobal,
    DefineGlobal,
    SetGlobal,
    GetUpvalue,
    SetUpvalue,
    GetProperty,
    SetProperty,
    GetSuper,
    Equal,
    Greater,
    Less,
    Add,
    Subtract,
    Multiply,
    Divide,
    Not,
    Negate,
    Print,
    Jump,
    JumpIfFalse,
    Loop,
    Call,
    SuperInvoke,
    Invoke,
    Closure,
    CloseUpvalue,
    Return,
    Class,
    Inherit,
    Method,
};

struct Chunk {
    i32 count;
    i32 capacity;
    u8* code;
    i32* lines;
    ValueArray constants;

    void init();
    void free();
    void write(u8 byte, i32 line);

    i32 add_constant(Value value);
};

}
