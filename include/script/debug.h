#pragma once

#include <script/chunk.h>

namespace calamus::script {

void disassemble_chunk(Chunk* chunk, const char* name);
int disassemble_instruction(Chunk* chunk, int offset);

}
