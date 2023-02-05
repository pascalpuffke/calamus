#include <script_legacy/memory.h>
#include <script_legacy/object.h>
#include <script_legacy/value.h>

namespace calamus::script {

void init_value_array(ValueArray* array) {
    array->values = nullptr;
    array->capacity = 0;
    array->count = 0;
}

void write_value_array(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        const auto old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = GROW_ARRAY(Value, array->values, static_cast<u64>(old_capacity), static_cast<u64>(array->capacity));
    }

    array->values[array->count++] = value;
}

void free_value_array(ValueArray* array) {
    FREE_ARRAY(Value, array->values, static_cast<u64>(array->capacity));
    init_value_array(array);
}

void print_value(Value value) {
#ifdef NAN_BOXING
    if (IS_BOOL(value))
        print(AS_BOOL(value) ? "true" : "false");
    else if (IS_NIL(value))
        print("nil");
    else if (IS_NUMBER(value))
        print(AS_NUMBER(value));
    else if (IS_OBJ(value))
        print_object(value);
#else
    switch (value.type) {
    case VAL_BOOL:
        print(AS_BOOL(value) ? "true" : "false");
        break;
    case VAL_NIL:
        print("nil");
        break;
    case VAL_NUMBER:
        print(AS_NUMBER(value));
        break;
    case VAL_OBJ:
        print_object(value);
        break;
    }
#endif
}

bool values_equal(Value a, Value b) {
#ifdef NAN_BOXING
    if (IS_NUMBER(a) && IS_NUMBER(b))
        return AS_NUMBER(a) == AS_NUMBER(b);
    return a == b;
#else
    if (a.type != b.type)
        return false;
    switch (a.type) {
    case VAL_BOOL:
        return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:
        return true;
    case VAL_NUMBER:
        return AS_NUMBER(a) == AS_NUMBER(b);
    case VAL_OBJ:
        return AS_OBJ(a) == AS_OBJ(b);
    default:
        UNREACHABLE();
    }
#endif
}

}
