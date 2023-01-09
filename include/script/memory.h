#pragma once

#include <script/config.h>
#include <script/object.h>
#include <util/types.h>

namespace calamus::script {

#define ALLOCATE(type, count) \
    reinterpret_cast<type*>(reallocate(NULL, 0, sizeof(type) * static_cast<size_t>(count)))

#define FREE(type, pointer) \
    reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(type, pointer, old_count, new_count) \
    reinterpret_cast<type*>(reallocate(pointer, sizeof(type) * (old_count), sizeof(type) * (new_count)))

#define FREE_ARRAY(type, pointer, old_count) \
    reallocate(pointer, sizeof(type) * (old_count), 0)

void* reallocate(void*, usize, usize);
void mark_object(Obj*);
void mark_value(Value);
void gc();
void free_objects();

}
