#pragma once

#include <script/chunk.h>
#include <script/config.h>
#include <script/table.h>
#include <script/value.h>
#include <string_view>
#include <utility>

namespace calamus::script {

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_BOUND_METHOD(value) is_obj_type(value, ObjType::BoundMethod)
#define IS_CLASS(value) is_obj_type(value, ObjType::Class)
#define IS_CLOSURE(value) is_obj_type(value, ObjType::Closure)
#define IS_FUNCTION(value) is_obj_type(value, ObjType::Function)
#define IS_INSTANCE(value) is_obj_type(value, ObjType::Instance)
#define IS_NATIVE(value) is_obj_type(value, ObjType::Native)
#define IS_STRING(value) is_obj_type(value, ObjType::String)

#define AS_BOUND_METHOD(value) (reinterpret_cast<ObjBoundMethod*>(AS_OBJ(value)))
#define AS_CLASS(value) (reinterpret_cast<ObjClass*>(AS_OBJ(value)))
#define AS_CLOSURE(value) (reinterpret_cast<ObjClosure*>(AS_OBJ(value)))
#define AS_FUNCTION(value) (reinterpret_cast<ObjFunction*>(AS_OBJ(value)))
#define AS_INSTANCE(value) (reinterpret_cast<ObjInstance*>(AS_OBJ(value)))
#define AS_NATIVE(value) (reinterpret_cast<ObjNative*>(AS_OBJ(value))->function)
#define AS_STRING(value) (reinterpret_cast<ObjString*>(AS_OBJ(value)))
#define AS_CSTRING(value) (reinterpret_cast<ObjString*>(AS_OBJ(value))->chars)

enum class ObjType {
    BoundMethod,
    Class,
    Closure,
    Function,
    Instance,
    Native,
    String,
    Upvalue
};

USED static std::string_view obj_type_to_string(ObjType type) {
    return (std::string_view[]) {
        "BoundMethod",
        "Class",
        "Closure",
        "Function",
        "Instance",
        "Native",
        "String",
        "Upvalue"
    }[std::to_underlying(type)];
}

struct Obj {
    ObjType type;
    bool is_marked;
    struct Obj* next;
};

struct ObjString {
    Obj obj;
    i32 length;
    char* chars;
    u32 hash;
};

struct ObjFunction {
    Obj obj;
    i32 arity;
    i32 upvalue_count;
    Chunk chunk;
    ObjString* name;
};

struct ObjUpvalue {
    Obj obj;
    Value* location;
    Value closed;
    struct ObjUpvalue* next;
};

struct ObjClosure {
    Obj obj;
    ObjFunction* function;
    ObjUpvalue** upvalues;
    i32 upvalue_count;
};

struct ObjClass {
    Obj obj;
    ObjString* name;
    Table methods;
};

struct ObjInstance {
    Obj obj;
    ObjClass* klass;
    Table fields;
};

struct ObjBoundMethod {
    Obj obj;
    Value receiver;
    ObjClosure* method;
};

typedef Value (*NativeFn)(int arg_count, Value* args);

struct ObjNative {
    Obj obj;
    NativeFn function;
};

ObjBoundMethod* new_bound_method(Value receiver, ObjClosure* method);
ObjClass* new_class(ObjString* name);
ObjInstance* new_instance(ObjClass* klass);
ObjClosure* new_closure(ObjFunction* function);
ObjFunction* new_function();
ObjUpvalue* new_upvalue(Value* slot);
ObjNative* new_native(NativeFn function);
ObjString* take_string(char* chars, i32 length);
ObjString* copy_string(const char* chars, i32 length);

void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

}
