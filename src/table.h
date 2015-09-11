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
Value hashtable_key_by_index(Hashtable* ht, int index);
Value hashtable_value_by_index(Hashtable* ht, int index);
bool hashtable_equals(Value left, Value right);

} // namespace ice
