// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

#include "tagged_value.h"

namespace ice {

#define ARRAY_DEFAULT_CAPACITY_FOR_EXTEND 8
#define ARRAY_ITERATOR_STACK_NODE_SIZE 8
#define ARRAY_CONS_SIZE 2

struct Array {
    u8 object_type; // TYPE_ARRAY
    u8 refcount;
    u16 length;
    u16 capacity;
    Value items[0];
};

struct ArrayNode {
    u8 object_type; // TYPE_ARRAY_NODE
    u8 refcount;
    u16 length;
    Value left;
    Value right;
};

struct ArraySlice {
    u8 object_type; // TYPE_ARRAY_SLICE
    u8 refcount;
    u16 length;

    u16 start_index;
    Value base;
};

Array* new_array(int length, int capacity);
Array* new_array(int length);
ArrayNode* new_array_node(Value left /*consumed*/, Value right /*consumed*/);
ArraySlice* new_array_slice(Value list /*consumed*/, int start_index, int length);

void free_array(Array* array);
void free_array_node(ArrayNode* node);
void free_array_slice(ArraySlice* node);

// 'array' has type (Array | ArrayNode)
void array_decref(Array* array);

// 'array' has type (Array | ArrayNode)
void array_incref(Array* array);

bool is_array(Value value);
bool is_array_node(Value value);
bool is_array_slice(Value value);

Value* array_get(Array* array, int index);
int array_length(Array* array);

struct ArrayIteratorStack {
    int length;
    Value items[ARRAY_ITERATOR_STACK_NODE_SIZE];
    ArrayIteratorStack* prev;
};

struct ArrayIterator {

    Value list;
    int index;
    ArrayIteratorStack* stack;

    ArrayIterator(Value);
    Value current();
    bool valid();
    void advance();
};

void array_iterator_start(ArrayIterator* it, Value list);
bool array_iterator_valid(ArrayIterator* it);
void array_iterator_advance(ArrayIterator* it);
Value array_iterator_current(ArrayIterator* it);

Value slice(Value list /*consumed*/, int start_index, int len);
Value first(Value list /* consumed */);
Value rest(Value list /* consumed */);
Value range(int start, int fin);

} // namespace ice
