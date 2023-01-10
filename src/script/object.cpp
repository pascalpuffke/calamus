#include <cstring>
#include <fmt/color.h>
#include <script/memory.h>
#include <script/object.h>
#include <script/value.h>
#include <script/vm.h>

namespace calamus::script {

#define ALLOCATE_OBJ(type, objectType) \
    reinterpret_cast<type*>(allocate_object(sizeof(type), objectType))

using namespace VM;

static Obj* allocate_object(size_t size, ObjType type) {
    auto* object = reinterpret_cast<Obj*>(reallocate(nullptr, 0, size));
    object->type = type;
    object->is_marked = false;
    object->next = vm.objects;
    vm.objects = object;

    if constexpr (DEBUG_LOG_GC) {
        print(fmt::fg(fmt::color::gray), "{} allocate {} bytes for type {}\n", fmt::ptr(object), size, obj_type_to_string(type));
    }

    return object;
}

ObjBoundMethod* new_bound_method(Value receiver, ObjClosure* method) {
    auto* bound = ALLOCATE_OBJ(ObjBoundMethod, ObjType::BoundMethod);
    bound->receiver = receiver;
    bound->method = method;
    return bound;
}

ObjClass* new_class(ObjString* name) {
    auto* klass = ALLOCATE_OBJ(ObjClass, ObjType::Class);
    klass->name = name;
    klass->methods.reset();
    return klass;
}

ObjInstance* new_instance(ObjClass* klass) {
    auto* instance = ALLOCATE_OBJ(ObjInstance, ObjType::Instance);
    instance->klass = klass;
    instance->fields.reset();
    return instance;
}

ObjClosure* new_closure(ObjFunction* function) {
    auto** upvalues = ALLOCATE(ObjUpvalue*, function->upvalue_count);
    std::memset(upvalues, 0, sizeof(ObjUpvalue*) * static_cast<u64>(function->upvalue_count));

    auto* closure = ALLOCATE_OBJ(ObjClosure, ObjType::Closure);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;
    return closure;
}

ObjFunction* new_function() {
    auto* function = ALLOCATE_OBJ(ObjFunction, ObjType::Function);
    function->arity = 0;
    function->upvalue_count = 0;
    function->name = nullptr;
    function->chunk.init();
    return function;
}

ObjUpvalue* new_upvalue(Value* slot) {
    auto* upvalue = ALLOCATE_OBJ(ObjUpvalue, ObjType::Upvalue);
    upvalue->closed = NIL_VAL;
    upvalue->location = slot;
    upvalue->next = nullptr;
    return upvalue;
}

ObjNative* new_native(NativeFn function) {
    auto* native = ALLOCATE_OBJ(ObjNative, ObjType::Native);
    native->function = function;
    return native;
}

static ObjString* allocate_string(char* chars, i32 length, u32 hash) {
    auto* string = ALLOCATE_OBJ(ObjString, ObjType::String);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    push(OBJ_VAL(string));
    vm.strings.set(string, NIL_VAL);
    pop();
    return string;
}

static u32 hash_string(const char* key, i32 length) {
    auto hash = 2166136261u;
    for (auto i = 0; i < length; i++) {
        hash ^= static_cast<u8>(key[i]);
        hash *= 16777619;
    }
    return hash;
}

ObjString* take_string(char* chars, i32 length) {
    auto hash = hash_string(chars, length);
    auto* interned = vm.strings.find_string(chars, length, hash);
    if (interned) {
        FREE_ARRAY(char, chars, static_cast<u64>(length + 1));
        return interned;
    }

    return allocate_string(chars, length, hash);
}

ObjString* copy_string(const char* chars, i32 length) {
    auto hash = hash_string(chars, length);
    auto* interned = vm.strings.find_string(chars, length, hash);
    if (interned)
        return interned;

    auto* heap_chars = ALLOCATE(char, length + 1);
    std::memcpy(heap_chars, chars, static_cast<usize>(length));
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length, hash);
}

static void print_function(ObjFunction* function) {
    if (function->name) {
        const auto name_view = std::string_view { function->name->chars, static_cast<std::string_view::size_type>(function->name->length) };
        print("<fn {}>", name_view);
        return;
    }
    print("<script>");
}

void print_object(Value value) {
    switch (OBJ_TYPE(value)) {
    case ObjType::BoundMethod:
        print_function(AS_BOUND_METHOD(value)->method->function);
        break;
    case ObjType::Class:
        print(AS_CLASS(value)->name->chars);
        break;
    case ObjType::Closure:
        print_function(AS_CLOSURE(value)->function);
        break;
    case ObjType::Function:
        print_function(AS_FUNCTION(value));
        break;
    case ObjType::Instance:
        print("{} instance", AS_INSTANCE(value)->klass->name->chars);
        break;
    case ObjType::Native:
        print("<native fn>");
        break;
    case ObjType::String:
        print(AS_CSTRING(value));
        break;
    case ObjType::Upvalue:
        print("upvalue");
        break;
    }
}

}
