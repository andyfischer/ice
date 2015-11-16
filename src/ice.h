// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

#include <stdint.h>

namespace ice {

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

struct Array;
struct ArrayNode;
struct ArraySlice;
struct FBlob;
struct Hashtable;
struct Value;
struct CValue;

struct ObjectHeader {
    u8 type;
    u8 refcount;
};

// The primary tagged value structure. Should be 64 bits on all platforms.
struct Value {
    union {
        u64 raw;
        void* ptr;
        Array* array_ptr;
        ArrayNode* array_node_ptr;
        ArraySlice* array_slice_ptr;
        Hashtable* hashtable_ptr;
        FBlob* fblob_ptr;
        ObjectHeader* object_ptr;

        // Small blob
        struct {
            u8 small_blob[7];
            u8 small_blob_len: 5;
            u8 tag_s: 3;
        };

        // Float 32
        struct {
            f32 f;
            u32 padding_f: 29;
            u8 tag_f: 3;
        };

        // Integer
        struct {
            i32 i;
            u32 padding_i: 29;
            u8 tag_i : 3;
        };

        // Extended tag
        struct {
            u8 extag: 8;
            u64 padding_ex: 53;
            u8 tag : 3;
        };
    };

    void dump();
    bool operator==(Value const& rhs) {
        return this->raw == rhs.raw;
    }
};

void init_value(Value* value);
void nullify(Value* value);

Value incref(Value value);
void decref(Value value);
void decref(Value value1, Value value2);
void decref(Value value1, Value value2, Value value3);
void decref(Value value1, Value value2, Value value3, Value value4);
void decref(Value value1, Value value2, Value value3, Value value4, Value value5);

// equals
//
// Returns whether 'lhs' and 'rhs' are exactly equal, aka, indistinguishable
// from each other.
bool equals(Value lhs, Value rhs);
bool equals_str(Value lhs, const char* str);
bool equals_symbol(Value value, const char* str);

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
bool is_null(Value value);
bool is_symbol(Value value);
bool is_table(Value value);

// Type checking (L0 types)
bool is_empty_list(Value value);
bool is_empty_table(Value value);
bool is_empty_blob(Value value);
bool is_hashtable(Value value);

// Value initialization
Value int_value(int i);
Value float_value(f32 f);
Value bool_value(bool b);
Value ptr_value(void*);
Value nullable_ptr_value(void*);
Value ex_value(u8 extag);
Value nil_value();
Value true_value();
Value false_value();
Value empty_list();
Value empty_blob();
Value empty_table();

// Integer
Value inc(Value i);

// List
Value list_0();
Value list_1(Value el1 /*consumed*/);
Value list_2(Value el1 /*consumed*/, Value el2 /*consumed*/);
Value list_3(Value el1 /*consumed*/, Value el2 /*consumed*/, Value el3 /*consumed*/);
Value list_4(Value el1 /*consumed*/, Value el2 /*consumed*/, Value el3 /*consumed*/,
    Value el4 /*consumed*/);

u32 length(Value list);
Value /*borrowed*/ get_index(Value list, int index);
Value take_index(Value list /*maybe modified*/, int index);
Value set_index(Value list /*consumed*/, int index, Value el /*consumed*/);

bool list_equals(Value left, Value right);
Value prepend(Value list /*consumed*/, Value prefix /*consumed*/);
Value append(Value list /*consumed*/, Value suffix /*consumed*/);
Value concat_n(Value items /*consumed*/);
Value concat_2(Value left /*consumed*/, Value right /*consumed*/);
Value slice(Value list /* consumed */, int start_index, int length);
Value first(Value list /* consumed */);
Value rest(Value list /* consumed */);
Value range(int start, int fin);

typedef Value(*map_f)(Value in /*consumed*/);
Value map(Value list /*consumed*/, map_f func);

// Table
Value table_0();
Value table_1(Value k /*consumed*/, Value v /*consumed*/);
Value table_2(Value k1 /*consumed*/, Value v1 /*consumed*/,
    Value k2 /*consumed*/, Value v2 /*consumed*/);
Value table_3(Value k1 /*consumed*/, Value v1 /*consumed*/,
    Value k2 /*consumed*/, Value v2 /*consumed*/,
    Value k3 /*consumed*/, Value v3 /*consumed*/);

Value get_key(Value table, Value key);
Value set_key(Value table /*consumed*/, Value key /*consumed*/, Value val /*consumed*/);
Value table_keys(Value table);
Value table_values(Value table);

// Blob
Value new_blob(u32 size);
Value from_str(const char* str);
Value blob_s(const char* str);
Value blob_p(void* ptr);
void* as_pointer(Value blob);
u32 blob_read_u32(Value blob, u64 byte_offset);
u64 blob_read_u64(Value blob, u64 byte_offset);
Value blob_append_s(Value blob /*consumed*/, const char* str);
Value blob_flatten(Value blob /*consumed*/);

// Flatten and null terminate 'blob', returning a char* to its data.
char* blob_cstr(Value* blob /*modified*/);

// symbol
Value symbol(const char* str);
Value to_symbol(Value value /*consumed*/);

// Lisp
Value parse(Value blob /*consumed*/);
Value parse_s(const char* str);
Value parse_multi(Value text /*consumed*/);
Value eval(Value expr /*consumed*/);

// Parse one s-expression.
Value parse(Value text /* consumed */);


} // namespace ice
