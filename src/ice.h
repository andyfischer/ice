// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

typedef struct Flat Flat;
typedef struct Slice Slice;
typedef struct Node Node;
typedef struct Value Value;
typedef struct ObjectHeader ObjectHeader;

// Block type
#define FLAT_BLOCK  1
#define SLICE_BLOCK 2
#define NODE_BLOCK  3

// Logical type
#define LIST_TYPE   1
#define TABLE_TYPE  2
#define BLOB_TYPE   3
#define SYMBOL_TYPE 4
#define TEXT_TYPE   5
#define INT_TYPE    6

// Data layout enum
#define TABLE_LAYOUT_UNINDEXED_LIST 1
#define TABLE_LAYOUT_INDEXED_LIST 2

#define TAG_OBJECT             0x0
#define TAG_OPAQUE_POINTER     0x1
#define TAG_EX                 0x2

#define EX_TAG_INT          0x0
#define EX_TAG_FLOAT        0x1
#define EX_TAG_NIL          0x2
#define EX_TAG_EMPTY_LIST   0x3
#define EX_TAG_EMPTY_TABLE  0x4
#define EX_TAG_EMPTY_BLOB   0x5
#define EX_TAG_TRUE         0x6
#define EX_TAG_FALSE        0x7

#define PACKED __attribute__((__packed__))

// The primary tagged value structure. 8 bytes in size.
typedef struct PACKED Value {
    union {
        u64 raw;
        void* ptr;
        ObjectHeader* object;
        Flat* flat;
        Slice* slice;
        Node* node;

        // Small blob
        struct {
            u8 small_blob[7];
            u8 small_blob_len: 5;
            u8 tag_s: 3;
        };

        // Float 32
        struct PACKED {
            f32 f;
            u32 padding_f: 21;
            u8 extag_f: 8;
            u8 tag_f: 3;
        };

        // Integer
        struct PACKED {
            i32 i;
            u32 padding_i: 21;
            u8 extag_i: 8;
            u8 tag_i : 3;
        };

        // Extended tag
        struct PACKED {
            u32 padding_ex_1 : 32;
            u32 padding_ex_2 : 21;
            u8 extag: 8;
            u8 tag : 3;
        };
    };

#ifdef __cplusplus
    void dump();
    bool operator==(Value const& right) { return this->raw == right.raw; }
    bool operator!=(Value const& right) { return this->raw != right.raw; }

    bool is_nil();
#endif

} Value;

typedef struct PACKED ObjectHeader {
    u8 block_type: 3;
    u8 logical_type: 3;
    u8 padding: 2;
    u8 refcount;
    u8 layout;
    u16 size;
} ObjectHeader;

typedef struct PACKED Flat {
    ObjectHeader header;
    u8 data[];
} Flat;

typedef struct PACKED Slice {
    ObjectHeader header;
    u16 start_pos;
    Value base;
} Slice;

typedef struct PACKED Node {
    ObjectHeader header;
    Value left;
    Value right;
} Node;

typedef Value (*func_1)(Value arg1);
typedef Value (*func_2)(Value arg1, Value arg2);
typedef void (*void_func_1)(Value arg1);
typedef void (*void_func_2)(Value arg1, Value arg2);

void nullify(Value* value);

Value incref(Value value);
void decref(Value value);
void decref2(Value value1, Value value2);
void decref3(Value value1, Value value2, Value value3);
void decref4(Value value1, Value value2, Value value3, Value value4);
void decref5(Value value1, Value value2, Value value3, Value value4, Value value5);

// Set the value to 'permanent', which will turn off refcounting so that the object
// is never deleted.
Value make_perm(Value value);
void free_perm(Value value);

// equals
//
// Returns whether 'left' and 'right' are exactly equal, aka, indistinguishable
// from each other.
bool equals(Value left, Value right);
bool equals_str(Value left, const char* str);
bool equals_symbol(Value value, const char* str);

bool shallow_equals(Value left, Value right);

Value deep_replace(Value value /*consumed*/, Value target, Value replacement);

void print(Value value);
void println(Value value);
void print_raw(Value value);
void dump(Value value);

Value stringify_append(Value buf /*consumed*/, Value val);
Value stringify(Value val);

u32 hashcode(Value val);

// Order comparison using default sorting. Returns -1 if 'left' should occur first, 1 if
// 'right' should occur first, and 0 if they have equal order.
int compare(Value left, Value right);

// A 'leaf' value does not contain or reference any other values.
bool is_leaf_value(Value value);

// - Type checking (L1 types)
bool is_bool(Value value);
bool is_blob(Value value);
bool is_int(Value value);
bool is_float(Value value);
bool is_list(Value value);
bool is_nil(Value value);
bool is_symbol(Value value);
bool is_table(Value value);
bool is_truthy(Value value);
bool is_opaque_pointer(Value value);

bool is_empty(Value value);
bool is_empty_list(Value value);
bool is_empty_table(Value value);
bool is_empty_blob(Value value);
bool is_hashtable(Value value);
bool is_object(Value value);

// Value initialization
Value int_value(int i);
Value float_value(f32 f);
Value bool_value(bool b);
Value ptr_value(void*);
Value opaque_ptr(void*);
Value ex_value(u8 extag);
Value nil_value();
Value true_value();
Value false_value();
Value empty_list();
Value empty_blob();
Value empty_table();

// Value access
void* as_opaque_pointer(Value p);

// Integer
Value increment(Value i);

// Float
f32 to_float(Value v);

// List
Value list0();
Value list1(Value el1 /*consumed*/);
Value list2(Value el1 /*consumed*/, Value el2 /*consumed*/);
Value list3(Value el1 /*consumed*/, Value el2 /*consumed*/, Value el3 /*consumed*/);
Value list4(Value el1 /*consumed*/, Value el2 /*consumed*/, Value el3 /*consumed*/,
    Value el4 /*consumed*/);

Value in_list(Value val);

u32 length(Value list);
Value /*borrowed*/ nth(Value list, int index);
Value take_nth(Value list /*maybe modified*/, int index);
Value set_nth(Value list /*consumed*/, int index, Value el /*consumed*/);
Value apply_nth(Value list, int index, func_1 func);

bool list_equals(Value left, Value right);
Value prepend(Value list /*consumed*/, Value prefix /*consumed*/);
Value append(Value list /*consumed*/, Value suffix /*consumed*/);
Value concat_n(Value items /*consumed*/);
Value concat(Value left /*consumed*/, Value right /*consumed*/);
//Value slice(Value list /* consumed */, int start_index, int length);
Value first(Value list /* consumed */);
Value rest(Value list /* consumed */);
void pop_first(Value list /*consumed*/, Value* first /*out*/, Value* rest /*out*/);
Value range(int start, int fin);

Value map(Value list /*consumed*/, func_1 func /* arg1 consumed */);
Value map_1(Value list /*consumed*/, func_2 func /* arg2 consumed */, Value arg1);
void each(Value list /*consumed*/, void_func_1 func /* consumes arg1 */);
void each_1(Value list /*consumed*/, void_func_2 func /* consumes arg2 */, Value arg1);
Value filter(Value list /*consumed*/, func_1 func /* arg1 readonly */);
Value filter_1(Value list /*consumed*/, func_2 func /* arg1 readonly */, Value arg1);

Value list_iterator_start(Value list);
bool list_iterator_valid(Value it);
Value list_iterator_advance(Value it /*consumed*/);
Value list_iterator_current(Value it);
Value list_iterator_take(Value it);

// Table
Value table0();
Value table1(Value k /*consumed*/, Value v /*consumed*/);
Value table2(Value k1 /*consumed*/, Value v1 /*consumed*/,
    Value k2 /*consumed*/, Value v2 /*consumed*/);
Value table3(Value k1 /*consumed*/, Value v1 /*consumed*/,
    Value k2 /*consumed*/, Value v2 /*consumed*/,
    Value k3 /*consumed*/, Value v3 /*consumed*/);
Value table4(Value k1 /*consumed*/, Value v1 /*consumed*/,
    Value k2 /*consumed*/, Value v2 /*consumed*/,
    Value k3 /*consumed*/, Value v3 /*consumed*/,
    Value k4 /*consumed*/, Value v4 /*consumed*/);
Value table5(Value k1 /*consumed*/, Value v1 /*consumed*/,
    Value k2 /*consumed*/, Value v2 /*consumed*/,
    Value k3 /*consumed*/, Value v3 /*consumed*/,
    Value k4 /*consumed*/, Value v4 /*consumed*/,
    Value k5 /*consumed*/, Value v5 /*consumed*/);

Value get(Value table, Value key);
Value get2(Value table, Value key1, Value key2);
Value set(Value table /*consumed*/, Value key /*consumed*/, Value val /*consumed*/);
Value set_path_a(Value obj /*consumed*/, Value* pathArr, int pathLen, Value el /*consumed*/);
Value take(Value obj /*maybe modified*/, Value key);
Value take_value(Value table, Value key);
Value insert(Value table /*consumed*/, Value key /*consumed*/, Value val /*consumed*/);
Value keys(Value table);
Value values(Value table);
Value delete_key(Value table /*consumed*/, Value key);

// Blob
Value from_str(const char* source);
Value to_cstr(Value blob /*modified*/);
char* as_cstr(Value blob);

// Symbol
Value symbol(const char* str);
Value gensym(Value value /*consumed*/);

// Primitives
i64 ice_atoi(Value blob);
f64 ice_atof(Value blob);

// Lisp
Value parse(Value blob /*consumed*/);
Value parse_s(const char* str);
Value parse_multi(Value text /*consumed*/);
Value eval(Value expr /*consumed*/);

// Parse one s-expression.
Value parse(Value text /* consumed */);

// File i/o
Value read_file(Value filename /*consumed*/);
Value write_file_if_different(Value filename /*consumed*/, Value contents);

#ifdef __cplusplus
} // extern "C"
#endif
