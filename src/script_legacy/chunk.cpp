#include <script_legacy/chunk.h>
#include <script_legacy/memory.h>
#include <script_legacy/vm.h>

namespace calamus::script {

void Chunk::init() {
    count = 0;
    capacity = 0;
    code = nullptr;
    lines = nullptr;
    init_value_array(&constants);
}

void Chunk::free() {
    FREE_ARRAY(u8, code, static_cast<usize>(capacity));
    FREE_ARRAY(int, lines, static_cast<usize>(capacity));
    free_value_array(&constants);
    init();
}

void Chunk::write(u8 byte, i32 line) {
    if (capacity < count + 1) {
        i32 old_capacity = capacity;
        capacity = GROW_CAPACITY(old_capacity);
        code = GROW_ARRAY(u8, code, static_cast<usize>(old_capacity), static_cast<usize>(capacity));
        lines = GROW_ARRAY(i32, lines, static_cast<usize>(old_capacity), static_cast<usize>(capacity));
    }

    code[count] = byte;
    lines[count] = line;
    count++;
}

i32 Chunk::add_constant(Value value) {
    VM::push(value);
    write_value_array(&constants, value);
    VM::pop();
    return constants.count - 1;
}

}
