#pragma once

#include <cstring>
#include <script_legacy/config.h>
#include <util/types.h>

namespace calamus::script {

typedef struct Obj Obj;
typedef struct ObjString ObjString;

#ifdef NAN_BOXING

#define SIGN_BIT (static_cast<u64>(0x8000000000000000))
#define QNAN (static_cast<u64>(0x7ffc000000000000))

#define TAG_NIL 1 // 01
#define TAG_FALSE 2 // 10
#define TAG_TRUE 3 // 11

typedef u64 Value;

static inline Value num_to_value(f64 num) {
    Value value;
    memcpy(&value, &num, sizeof(f64));
    return value;
}

static inline f64 value_to_num(Value value) {
    f64 num;
    memcpy(&num, &value, sizeof(Value));
    return num;
}

#define IS_BOOL(value) (((value) | 1) == TRUE_VAL)
#define IS_NIL(value) ((value) == NIL_VAL)
#define IS_NUMBER(value) (((value)&QNAN) != QNAN)
#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define AS_BOOL(value) ((value) == TRUE_VAL)
#define AS_NUMBER(value) value_to_num(value)
#define AS_OBJ(value) \
    (reinterpret_cast<Obj*>(static_cast<uintptr_t>(((value) & ~(SIGN_BIT | QNAN)))))

#define BOOL_VAL(b) ((b) ? TRUE_VAL : FALSE_VAL)
#define FALSE_VAL \
    (static_cast<Value>(static_cast<u64>((QNAN | TAG_FALSE))))
#define TRUE_VAL \
    (static_cast<Value>(static_cast<u64>((QNAN | TAG_TRUE))))
#define NIL_VAL \
    (static_cast<Value>(static_cast<u64>((QNAN | TAG_NIL))))
#define NUMBER_VAL(num) num_to_value(num)
#define OBJ_VAL(obj) \
    static_cast<Value>((SIGN_BIT | QNAN | static_cast<u64>(reinterpret_cast<uintptr_t>((obj)))))

#else

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        f64 number;
        Obj* obj;
    } as;
} Value;

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJ(value) ((value).type == VAL_OBJ)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)

#define BOOL_VAL(value) ((Value) { VAL_BOOL, { .boolean = value } })
#define NIL_VAL ((Value) { VAL_NIL, { .number = 0 } })
#define NUMBER_VAL(value) ((Value) { VAL_NUMBER, { .number = value } })
#define OBJ_VAL(object) ((Value) { VAL_OBJ, { .obj = (Obj*)object } })

#endif

typedef struct {
    i32 capacity;
    i32 count;
    Value* values;
} ValueArray;

bool values_equal(Value a, Value b);

void init_value_array(ValueArray*);
void write_value_array(ValueArray*, Value);
void free_value_array(ValueArray*);

void print_value(Value);

}
