#include <fmt/color.h>
#include <script/debug.h>
#include <script/object.h>
#include <script/value.h>

namespace calamus::script {

static i32 simple_instruction(const char* name, i32 offset) {
    println("{}", name);
    return offset + 1;
}

static i32 byte_instruction(const char* name, Chunk* chunk, i32 offset) {
    const auto slot = chunk->code[offset + 1];
    println("{:<16} {:4d}", name, fmt::styled(slot, fmt::fg(fmt::color::gray)));
    return offset + 2;
}

static i32 jump_instruction(const char* name, i32 sign, Chunk* chunk, i32 offset) {
    auto jump = static_cast<u16>((chunk->code[offset + 1]) << 8);
    jump |= chunk->code[offset + 2];
    println("{:<16} {:4d} -> {:d}", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

static i32 constant_instruction(const char* name, Chunk* chunk, i32 offset) {
    const auto constant = chunk->code[offset + 1];
    fmt::print("{:<16} {:4d} '", name, fmt::styled(constant, fmt::fg(fmt::color::gray)));
    print_value(chunk->constants.values[constant]);
    println("'");
    return offset + 2;
}

static i32 invoke_instruction(const char* name, Chunk* chunk, i32 offset) {
    const auto constant = chunk->code[offset + 1];
    const auto arg_count = chunk->code[offset + 2];
    fmt::print("{:<16} ({} args) {:4d} '", name, arg_count, constant);
    print_value(chunk->constants.values[constant]);
    println("'");
    return offset + 3;
}

void disassemble_chunk(Chunk* chunk, const char* name) {
    println("==[{}]==", fmt::styled(name, fmt::text_style(fmt::emphasis::bold)));

    for (auto offset = 0; offset < chunk->count;) {
        offset = disassemble_instruction(chunk, offset);
    }
}

i32 disassemble_instruction(Chunk* chunk, i32 offset) {
    fmt::print("{:>04} ", fmt::styled(offset, fmt::fg(fmt::color::gray)));
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        fmt::print(fmt::fg(fmt::color::light_gray), "   | ");
    } else {
        fmt::print("{:>4} ", chunk->lines[offset]);
    }

    const auto instruction = static_cast<OpCode>(chunk->code[offset]);
    switch (instruction) {
    case OpCode::Constant:
        return constant_instruction("Constant", chunk, offset);
    case OpCode::Nil:
        return simple_instruction("Nil", offset);
    case OpCode::True:
        return simple_instruction("True", offset);
    case OpCode::False:
        return simple_instruction("False", offset);
    case OpCode::Pop:
        return simple_instruction("Pop", offset);
    case OpCode::GetLocal:
        return byte_instruction("GetLocal", chunk, offset);
    case OpCode::SetLocal:
        return byte_instruction("SetLocal", chunk, offset);
    case OpCode::GetGlobal:
        return constant_instruction("GetGlobal", chunk, offset);
    case OpCode::DefineGlobal:
        return constant_instruction("DefineGlobal", chunk, offset);
    case OpCode::SetGlobal:
        return constant_instruction("SetGlobal", chunk, offset);
    case OpCode::GetUpvalue:
        return byte_instruction("GetUpvalue", chunk, offset);
    case OpCode::SetUpvalue:
        return byte_instruction("SetUpvalue", chunk, offset);
    case OpCode::GetProperty:
        return constant_instruction("GetProperty", chunk, offset);
    case OpCode::SetProperty:
        return constant_instruction("SetProperty", chunk, offset);
    case OpCode::GetSuper:
        return constant_instruction("GetSuper", chunk, offset);
    case OpCode::Equal:
        return simple_instruction("Equal", offset);
    case OpCode::Greater:
        return simple_instruction("Greater", offset);
    case OpCode::Less:
        return simple_instruction("Less", offset);
    case OpCode::Add:
        return simple_instruction("Add", offset);
    case OpCode::Subtract:
        return simple_instruction("Subtract", offset);
    case OpCode::Multiply:
        return simple_instruction("Multiply", offset);
    case OpCode::Divide:
        return simple_instruction("Divide", offset);
    case OpCode::Not:
        return simple_instruction("Not", offset);
    case OpCode::Negate:
        return simple_instruction("Negate", offset);
    case OpCode::Print:
        return simple_instruction("Print", offset);
    case OpCode::Jump:
        return jump_instruction("Jump", 1, chunk, offset);
    case OpCode::JumpIfFalse:
        return jump_instruction("JumpIfFalse", 1, chunk, offset);
    case OpCode::Loop:
        return jump_instruction("Loop", -1, chunk, offset);
    case OpCode::Call:
        return byte_instruction("Call", chunk, offset);
    case OpCode::Invoke:
        return invoke_instruction("Invoke", chunk, offset);
    case OpCode::SuperInvoke:
        return invoke_instruction("SuperInvoke", chunk, offset);
    case OpCode::Closure: {
        offset++;
        const auto constant = chunk->code[offset++];
        fmt::print("{:<16} {:4d} '", "Closure", fmt::styled(constant, fmt::fg(fmt::color::gray)));
        print_value(chunk->constants.values[constant]);
        println();
        ObjFunction* function = AS_FUNCTION(chunk->constants.values[constant]);
        for (auto j = 0; j < function->upvalue_count; j++) {
            const auto is_local = chunk->code[offset++];
            const auto index = chunk->code[offset++];
            println("{:<04}      |                     {} {}", offset - 2, is_local ? "local" : "upvalue", index);
        }
        return offset;
    }
    case OpCode::CloseUpvalue:
        return simple_instruction("CloseUpvalue", offset);
    case OpCode::Return:
        return simple_instruction("Return", offset);
    case OpCode::Class:
        return constant_instruction("Class", chunk, offset);
    case OpCode::Inherit:
        return constant_instruction("Inherit", chunk, offset);
    case OpCode::Method:
        return constant_instruction("Method", chunk, offset);
    default:
        println("Unknown opcode {}", static_cast<u8>(instruction));
        return offset + 1;
    }
}

}
