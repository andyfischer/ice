// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "list.h"
#include "table.h"
#include "tagged_value.h"

namespace ice {

const int NEW_TABLE_DEFAULT_CAPACITY = 8;

HashtablePair* Hashtable::getPair(int pindex)
{
    ice_assert(pindex >= 0 && pindex < this->capacity);
    HashtablePair* first_pair = (HashtablePair*) &this->buckets[this->capacity];
    return first_pair + pindex;
}

Hashtable* new_hashtable(u16 capacity)
{
    Hashtable* table = (Hashtable*) ice_malloc(sizeof(Hashtable)
        + sizeof(BucketItem) * capacity
        + sizeof(HashtablePair) * capacity);

    table->object_type = TYPE_HASHTABLE;
    table->refcount = 1;
    table->length = 0;
    table->capacity = capacity;

    for (int i=0; i < capacity; i++)
        table->buckets[i].pindex = -1;

    return table;
}

void free_hashtable(Hashtable* hashtable)
{
    for (int i=0; i < hashtable->length; i++) {
        HashtablePair* pair = hashtable->getPair(i);
        decref(pair->key);
        decref(pair->value);
    }
    free(hashtable);
}

Value hashtable_key_by_index(Hashtable* ht, int index) { return ht->getPair(index)->key; }
Value hashtable_value_by_index(Hashtable* ht, int index) { return ht->getPair(index)->value; }

bool hashtable_equals(Value left, Value right)
{
    if (!is_hashtable(right))
        return false;

    ice_assert(is_hashtable(left));
    ice_assert(is_hashtable(right));

    Hashtable* left_ht = left.hashtable_ptr;
    Hashtable* right_ht = right.hashtable_ptr;

    if (left_ht->length != right_ht->length)
        return false;

    for (int i=0; i < left_ht->length; i++) {
        HashtablePair* leftPair = left_ht->getPair(i);
        HashtablePair* rightPair = left_ht->getPair(i);
        if (!equals(leftPair->key, rightPair->key))
            return false;
        if (!equals(leftPair->value, rightPair->value))
            return false;
    }
    return true;
}

int claim_next_bucket(Hashtable* ht, int currentIndex)
{
    int nextIndex = currentIndex;
    while (1) {
        nextIndex = (nextIndex + 1) % ht->capacity;

        ice_assert(nextIndex != currentIndex);

        if (ht->buckets[nextIndex].pindex == -1) {
            ht->buckets[currentIndex].next_bucket = nextIndex;
            return nextIndex;
        }
    }

    ice_assert(false); // unreachable
    return 0;
}

void insert(Hashtable* ht, Value key /*consumed*/, Value value /*consumed*/)
{
    ice_assert(ht->length < ht->capacity);

    u32 hash = hashcode(key);
    int bucketIndex = hash % (ht->capacity);

    while (1) {
        BucketItem bucketItem = ht->buckets[bucketIndex];

        if (bucketItem.pindex == -1) {
            // This bucket is available.

            int pindex = ht->length++;
            ht->buckets[bucketIndex].pindex = pindex;
            ht->buckets[bucketIndex].next_bucket = -1;
            ht->buckets[bucketIndex].hashcode = hash;

            HashtablePair* pair = ht->getPair(pindex);
            pair->key = key;
            pair->value = value;
            return;
        }

        // Bucket is taken. Do we have an equal key?
        if (hash == bucketItem.hashcode) {
            HashtablePair* existingPair = ht->getPair(bucketItem.pindex);
            if (equals(key, existingPair->key)) {
                // Key is equal, this is our pair.
                decref(key);
                decref(existingPair->value);
                existingPair->value = value;
                return;
            }
        }

        // Bucket is taken and it's not ours.
        if (bucketItem.next_bucket != -1) {
            // Loop, look at next bucket.
            bucketIndex = bucketItem.next_bucket;
            continue;
        }

        // Claim next available bucket.
        bucketIndex = claim_next_bucket(ht, bucketIndex);
    }
}

Value /*borrowed*/ find(Hashtable* ht, Value key)
{
    u32 hash = hashcode(key);
    int bucketIndex = hash % (ht->capacity);

    while (1) {
        BucketItem bucketItem = ht->buckets[bucketIndex];
        if (bucketItem.pindex == -1)
            return nil_value();

        if (hash == bucketItem.hashcode) {
            HashtablePair* pair = ht->getPair(bucketItem.pindex);
            if (equals(key, pair->key))
                return pair->value;
        }

        bucketIndex = bucketItem.next_bucket;
        if (bucketIndex == -1)
            return nil_value();
    }
}

Value insert(Value table, Value key, Value val)
{
    if (is_empty_table(table)) {
        Hashtable* ht = new_hashtable(NEW_TABLE_DEFAULT_CAPACITY);
        insert(ht, key, val);
        return ptr_value(ht);
    }

    ice_assert(is_hashtable(table));

    Hashtable* ht = table.hashtable_ptr;

    if (ht->length < ht->capacity) {
        if (object_is_writeable(table)) {
            // Modify in place
            insert(ht, key, val);
            return ptr_value(ht);
        } else {
            // Create modified copy
            Hashtable* modified_ht = new_hashtable(ht->capacity);
            for (int i=0; i < ht->length; i++) {
                HashtablePair* pair = ht->getPair(i);
                insert(modified_ht, incref(pair->key), incref(pair->value));
            }
            insert(modified_ht, key, val);
            decref(table);
            return ptr_value(modified_ht);
        }
    }

    // Allocate bigger list
    Hashtable* bigger_ht = new_hashtable(ht->capacity * 2);

    if (object_is_writeable(table)) {
        // Steal all keys & values
        for (int i=0; i < ht->length; i++) {
            HashtablePair* pair = ht->getPair(i);
            insert(bigger_ht, pair->key, pair->value);
        }
        insert(bigger_ht, key, val);
        free(ht);
        return ptr_value(bigger_ht);

    } else {
        // Copy all keys & values
        for (int i=0; i < ht->length; i++) {
            HashtablePair* pair = ht->getPair(i);
            insert(bigger_ht, incref(pair->key), incref(pair->value));
        }
        insert(bigger_ht, key, val);
        decref(table);
        return ptr_value(bigger_ht);
    }
}

Value set_key(Value table, Value key, Value val)
{
    return insert(table, key, val);
}

Value table_0()
{
    return empty_table();
}

Value table_1(Value k /*consumed*/, Value v /*consumed*/)
{
    Hashtable* ht = new_hashtable(1);
    insert(ht, k, v);
    return ptr_value(ht);
}

Value table_2(Value k1, Value v1, Value k2, Value v2)
{
    Hashtable* ht = new_hashtable(2);
    insert(ht, k1, v1);
    insert(ht, k2, v2);
    return ptr_value(ht);
}

Value table_3(Value k1, Value v1, Value k2, Value v2, Value k3, Value v3)
{
    Hashtable* ht = new_hashtable(3);
    insert(ht, k1, v1);
    insert(ht, k2, v2);
    insert(ht, k3, v3);
    return ptr_value(ht);
}

Value get_key(Value table, Value key)
{
    if (is_empty_table(table))
        return nil_value();

    if (is_hashtable(table))
        return find(table.hashtable_ptr, key);

    return nil_value();
}

Value table_keys_or_values(Value table, int key_or_value)
{
    if (is_empty_table(table))
        return empty_list();

    ice_assert(is_hashtable(table));

    Hashtable* ht = table.hashtable_ptr;
    Array* array = new_array(ht->length);
    for (int i=0; i < ht->length; i++)
        array->items[i] = incref(ht->getPair(i)->pair[key_or_value]);

    return ptr_value(array);
}

Value table_keys(Value table) { return table_keys_or_values(table, 0); }
Value table_values(Value table) { return table_keys_or_values(table, 1); }

} // namespace ice
