#include <cstdlib>
#include <fmt/color.h>
#include <script/compiler.h>
#include <script/debug.h>
#include <script/memory.h>
#include <script/vm.h>

namespace calamus::script {

constexpr static auto GC_HEAP_GROW_FACTOR = usize { 2 };

using namespace calamus::script::VM;

void* reallocate(void* pointer, usize old_size, usize new_size) {
    vm.bytes_allocated += new_size - old_size;
    if constexpr (DEBUG_STRESS_GC) {
        if (new_size > old_size)
            gc();
    }

    if (vm.bytes_allocated > vm.next_gc)
        gc();

    if (new_size == 0) {
        free(pointer);
        return nullptr;
    }

    return realloc(pointer, new_size); // If we run out of memory and realloc() returns 0, too bad.
}

static void free_object(Obj* object) {
    if constexpr (DEBUG_LOG_GC) {
        print(fmt::fg(fmt::color::gray), "{} free type {}\n", fmt::ptr(object), obj_type_to_string(object->type));
    }

    switch (object->type) {
    case ObjType::BoundMethod:
        FREE(ObjBoundMethod, object);
        break;
    case ObjType::Class: {
        auto* klass = reinterpret_cast<ObjClass*>(object);
        klass->methods.free();
        FREE(ObjClass, object);
        break;
    }
    case ObjType::Closure: {
        auto* closure = reinterpret_cast<ObjClosure*>(object);
        FREE_ARRAY(ObjUpvalue*, closure->upvalues, static_cast<u64>(closure->upvalue_count));
        FREE(ObjClosure, object);
        break;
    }
    case ObjType::Function: {
        auto* function = reinterpret_cast<ObjFunction*>(object);
        function->chunk.free();
        FREE(ObjFunction, object);
        break;
    }
    case ObjType::Instance: {
        auto* instance = reinterpret_cast<ObjInstance*>(object);
        instance->fields.free();
        FREE(ObjInstance, object);
        break;
    }
    case ObjType::Native:
        FREE(ObjNative, object);
        break;
    case ObjType::String: {
        auto* string = reinterpret_cast<ObjString*>(object);
        FREE_ARRAY(char, string->chars, static_cast<u64>(string->length + 1));
        FREE(ObjString, object);
        break;
    }
    case ObjType::Upvalue:
        FREE(ObjUpvalue, object);
        break;
    }
}

void free_objects() {
    auto* object = vm.objects;
    while (object) {
        auto* next = object->next;
        free_object(object);
        object = next;
    }

    free(vm.gray_stack);
}

void mark_object(Obj* object) {
    // Helping branch prediction this way improves performance more than I expected.
    // Just these two can help by up to 13%.
    if (!object) [[likely]]
        return;
    if (object->is_marked) [[likely]]
        return;

    if constexpr (DEBUG_LOG_GC) {
        print("{} mark ", fmt::ptr(object));
        print_value(OBJ_VAL(object));
        println();
    }

    object->is_marked = true;

    if (vm.gray_capacity < vm.gray_count + 1) {
        vm.gray_capacity = GROW_CAPACITY(vm.gray_capacity);
        vm.gray_stack = reinterpret_cast<Obj**>(realloc(vm.gray_stack, sizeof(Obj*) * static_cast<u64>(vm.gray_capacity)));

        if (!vm.gray_stack)
            exit(1);
    }

    vm.gray_stack[vm.gray_count++] = object;
}

void mark_value(Value value) {
    if (!IS_OBJ(value))
        return;
    mark_object(AS_OBJ(value));
}

static void mark_roots() {
    for (auto* slot = vm.stack; slot < vm.stack_top; slot++) {
        mark_value(*slot);
    }

    for (auto i = 0; i < vm.frame_count; i++) {
        mark_object(reinterpret_cast<Obj*>(vm.frames[i].closure));
    }

    for (auto* upvalue = vm.open_upvalues; upvalue; upvalue = upvalue->next) {
        mark_object(reinterpret_cast<Obj*>(upvalue));
    }

    vm.globals.mark();
    mark_compiler_roots();
    mark_object(reinterpret_cast<Obj*>(vm.init_string));
}

static void mark_array(ValueArray* array) {
    for (auto i = 0; i < array->count; i++) {
        mark_value(array->values[i]);
    }
}

static void blacken_object(Obj* object) {
    if constexpr (DEBUG_LOG_GC) {
        print("{} blacken ", fmt::ptr(object));
        print_value(OBJ_VAL(object));
        println();
    }

    switch (object->type) {
    case ObjType::BoundMethod: {
        auto* bound = reinterpret_cast<ObjBoundMethod*>(object);
        mark_value(bound->receiver);
        mark_object(reinterpret_cast<Obj*>(bound->method));
        break;
    }
    case ObjType::Class: {
        auto* klass = reinterpret_cast<ObjClass*>(object);
        mark_object(reinterpret_cast<Obj*>(klass->name));
        klass->methods.mark();
        break;
    }
    case ObjType::Closure: {
        auto* closure = reinterpret_cast<ObjClosure*>(object);
        mark_object(reinterpret_cast<Obj*>(closure->function));
        for (auto i = 0; i < closure->upvalue_count; i++) {
            mark_object(reinterpret_cast<Obj*>(closure->upvalues[i]));
        }
        break;
    }
    case ObjType::Function: {
        auto* function = reinterpret_cast<ObjFunction*>(object);
        mark_object(reinterpret_cast<Obj*>(function->name));
        mark_array(&function->chunk.constants);
        break;
    }
    case ObjType::Instance: {
        auto* instance = reinterpret_cast<ObjInstance*>(object);
        mark_object(reinterpret_cast<Obj*>(instance->klass));
        instance->fields.mark();
        break;
    }
    case ObjType::Upvalue:
        mark_value(reinterpret_cast<ObjUpvalue*>(object)->closed);
        break;
    case ObjType::Native:
    case ObjType::String:
        break;
    }
}

static void trace_references() {
    while (vm.gray_count > 0) {
        auto* object = vm.gray_stack[--vm.gray_count];
        blacken_object(object);
    }
}

static void sweep() {
    Obj* previous = nullptr;
    Obj* object = vm.objects;
    while (object) {
        if (object->is_marked) {
            object->is_marked = false;
            previous = object;
            object = object->next;
        } else {
            auto* unreached = object;
            object = object->next;
            if (previous)
                previous->next = object;
            else
                vm.objects = object;
            free_object(unreached);
        }
    }
}

void gc() {
    [[maybe_unused]] usize before;
    if constexpr (DEBUG_LOG_GC) {
        println("-- gc begin");
        before = vm.bytes_allocated;
    }

    mark_roots();
    trace_references();
    vm.strings.remove_white();
    sweep();

    vm.next_gc = vm.bytes_allocated * GC_HEAP_GROW_FACTOR;

    if constexpr (DEBUG_LOG_GC) {
        println("-- gc end");
        println("   collected {} bytes (from {} to {}) next at {}", before - vm.bytes_allocated, before, vm.bytes_allocated, vm.next_gc);
    }
}

}
