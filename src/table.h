// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

#include "tagged_value.h"

// notes:
// keep the ordered section

namespace ice {

struct BucketItem {
    i16 pindex;
    i16 next_bucket;
    u32 hashcode;
};

struct HashtablePair {
    union {
        struct {
            Value key;
            Value value;
        };

        Value pair[2];
    };
};

struct Hashtable {
    u8 object_type; // TYPE_HASHTABLE
    u8 refcount;
    u16 length;
    u16 capacity;

    BucketItem buckets[0]; // buckets[capacity]
    // followed by: HashtablePair pairs[capacity];

    HashtablePair* getPair(int pindex);
};

Hashtable* new_hashtable(u16 capacity);
void free_hashtable(Hashtable* hashtable);
Hashtable* hashtable_shallow_copy(Hashtable* ht);
Value hashtable_key_by_index(Hashtable* ht, int index);
Value hashtable_value_by_index(Hashtable* ht, int index);
bool hashtable_equals(Value left, Value right);

Value get_key_by_index(Value table, int index);
Value delete_key(Value table /*consumed*/, Value key);

#if 0
struct TableIterator {
    Value table;
    int index;

    ArrayIterator(Value);
    Value key() { return table_iterator_current_key(this); }
    Value value() { return table_iterator_current_value(this); }
    bool valid() { return table_iterator_valid(this); }
    void advance() { return table_iterator_advance(this); }
};

void table_iterator_start(TableIterator* it, Value table);
bool table_iterator_valid(TableIterator* it);
void table_iterator_advance(TableIterator* it);
Value table_iterator_current_key(TableIterator* it);
Value table_iterator_current_value(TableIterator* it);
#endif

} // namespace ice
