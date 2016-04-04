
#pragma once

#include "value.h"


#if 0
typedef struct Bucket_ {
    i16 pindex;
    i16 next_bucket;
    u32 hashcode;
} Bucket;

typedef struct Pair_ {
    union {
        struct {
            Value key;
            Value value;
        };

        Value pair[2];
    };
} Pair;

typedef struct Hashtable_ {
    u8 object_type; // TYPE_HASHTABLE
    u8 refcount;
    u16 length;
    u16 capacity;

    Bucket buckets[]; // buckets[capacity]
    // followed by: Pair pairs[capacity];
} Hashtable;

typedef struct HashedList_ {
    u8 object_type; // TYPE_HASHTABLE
    u8 refcount;
    u16 capacity;
    Value list;
    Bucket buckets[]; // buckets[capacity]
} HashedList;

Hashtable* new_hashtable(u16 capacity);
void free_hashtable(Hashtable* hashtable);
Hashtable* hashtable_shallow_copy(Hashtable* ht);

Value* hashtable_find(Hashtable* ht, Value key);
Pair* hashtable_get_pair(Hashtable* ht, int pindex);

Value new_hashed_list(Value list /*consumed*/);

Value hashtable_key_by_index(Hashtable* ht, int index);
Value hashtable_value_by_index(Hashtable* ht, int index);
bool hashtable_equals(Value left, Value right);

Value delete_key(Value table /*consumed*/, Value key);

#endif
