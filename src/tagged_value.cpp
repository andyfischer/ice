// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "blob.h"
#include "list.h"
#include "table.h"
#include "tagged_value.h"

using namespace ice;

namespace ice {

void init_value(Value* value)
{
    *value = nil_value();
}

void nullify(Value* value)
{
    decref(*value);
    *value = nil_value();
}

Value incref(Value value)
{
    if (is_object(value))
        value.object_ptr->refcount++;
    return value;
}

void decref(Value value)
{
    if (!is_object(value))
        return;

    ice_assert(value.object_ptr->refcount > 0);

    if (value.object_ptr->refcount == 1) {
        switch (value.object_ptr->type) {
        case TYPE_ARRAY:
            free_array(value.array_ptr);
            return;
        case TYPE_ARRAY_NODE:
            free_array_node(value.array_node_ptr);
            return;
        case TYPE_ARRAY_SLICE:
            free_array_slice(value.array_slice_ptr);
            return;
        case TYPE_FBLOB:
        case TYPE_FBLOB_SYMBOL:
            free_fblob(value.fblob_ptr);
            return;
        case TYPE_HASHTABLE:
            free_hashtable(value.hashtable_ptr);
            return;
        }
        return;
    }

    value.object_ptr->refcount--;
}

void decref(Value value1, Value value2)
{
    decref(value1);
    decref(value2);
}

void decref(Value value1, Value value2, Value value3)
{
    decref(value1);
    decref(value2);
    decref(value3);
}

void decref(Value value1, Value value2, Value value3, Value value4)
{
    decref(value1);
    decref(value2);
    decref(value3);
    decref(value4);
}

void decref(Value value1, Value value2, Value value3, Value value4, Value value5)
{
    decref(value1);
    decref(value2);
    decref(value3);
    decref(value4);
    decref(value5);
}

bool is_object(Value value)
{
    return value.tag == 0;
}

bool object_is_writeable(Value value)
{
    ice_assert(is_object(value));
    return value.object_ptr->refcount == 1;
}

bool is_writeable_object(Value value)
{
    return is_object(value) && value.object_ptr->refcount == 1;
}

int refcount(Value value)
{
    if (!is_object(value))
        return 1;
    return value.object_ptr->refcount;
}

inline bool is_ex_tag(Value value)
{
    return value.tag == TAG_EX;
}

bool shallow_equals(Value lhs, Value rhs)
{
    return lhs.raw == rhs.raw;
}

bool equals_int(Value value, int i)
{
    if (!is_int(value))
        return false;
    return value.i == i;
}

bool equals_symbol(Value value, const char* str)
{
    if (!is_symbol(value))
        return false;

    return blob_equals_str(value, str);
}

bool equals(Value left, Value right)
{
    if (shallow_equals(left, right))
        return true;

    if (is_blob(left) || is_symbol(left))
        return blob_equals(left, right);

    if (is_list(left))
        return list_equals(left, right);

    if (is_hashtable(left))
        return hashtable_equals(left, right);

    return false;
}

bool equals_str(Value lhs, const char* str)
{
    if (is_blob(lhs))
        return blob_equals_str(lhs, str);
    return false;
}

Value deep_replace(Value obj /*consumed*/, Value target, Value replacement)
{
    if (equals(obj, target)) {
        decref(obj);
        return incref(replacement);
    }

    if (is_list(obj)) {
        for (int i=0; i < length(obj); i++) {
            if (equals(get_index(obj, i), target))
                obj = set_index(obj, i, replacement);
        }
    } else if (is_table(obj)) {

        for (int i=0; i < length(obj); i++) {
            Value key = get_key_by_index(obj, i);
            if (equals(key, target)) {
                obj = delete_key(obj, key);
                obj = insert(obj, target, replacement);
            }

            Value val = get_index(obj, i);
            if (equals(val, target))
                obj = set_value_by_index(obj, i, replacement);
        }
    }

    return obj;
}

u32 hashcode(Value val)
{
    u32 result = 0;

    if (is_list(val)) {
        result = (u32) (EX_TAG_EMPTY_LIST << 8);
        u32 len = length(val);
        for (int i=0; i < len; i++)
            result ^= hashcode(get_index(val, i));
    } else if (is_blob(val) || is_symbol(val)) {
        int offset = 0;
        for (ByteIterator it(val); it.valid(); it.advance()) {
            result ^= it.current() << (offset*3);
            offset = (offset + 1) % 4;
        }
    } else {
        result = ((u32) val.raw) ^ (val.raw >> 32);
    }

    // Don't allow a hashcode of 0, since 0 has special meaning of "not computed".
    result += (result == 0) * 1;
    return result;
}

void print(Value value)
{
    if (is_blob(value)) {
        blob_print(value);
        return;
    }

    Value rep;
    rep = stringify(value);
    blob_print(rep);
    decref(rep);
}

void println(Value value)
{
    print(value);
    printf("\n");
}

Value stringify_append(Value buf, Value suffix)
{
    ice_assert(is_blob(buf));

    switch (suffix.tag) {
    case TAG_POINTER:
        switch (suffix.object_ptr->type) {

        case TYPE_FBLOB:
            buf = blob_append_s(buf, "\"", 1);
            buf = blob_append(buf, suffix);
            buf = blob_append_s(buf, "\"", 1);
            return buf;

        case TYPE_FBLOB_SYMBOL: {
            buf = blob_append_s(buf, ":", 1);
            u32 suffix_len = suffix.fblob_ptr->size;
            char* new_section = blob_append_writeable_section(&buf, suffix_len);
            memcpy(new_section, suffix.fblob_ptr->data, suffix_len);
            return buf;
        }

        case TYPE_HASHTABLE: {
            Hashtable* ht = suffix.hashtable_ptr;
            buf = blob_append_s(buf, "{", 1);
            for (int i=0; i < ht->length; i++) {
                if (i != 0)
                    buf = blob_append_s(buf, ", ", 2);
                buf = stringify_append(buf, hashtable_key_by_index(ht, i));
                buf = blob_append_s(buf, " -> ", 4);
                buf = stringify_append(buf, hashtable_value_by_index(ht, i));
            }
            buf = blob_append_s(buf, "}", 1);
            return buf;
        }

        case TYPE_ARRAY:
        case TYPE_ARRAY_NODE:
        case TYPE_ARRAY_SLICE: {
            buf = blob_append_s(buf, "[", 1);

            bool first = true;
            for (ArrayIterator it(suffix); it.valid(); it.advance()) {
                if (!first)
                    buf = blob_append_s(buf, ", ", 2);
                buf = stringify_append(buf, it.current());
                first = false;
            }

            buf = blob_append_s(buf, "]", 1);
            return buf;
        }
        }

        break;

    case TAG_INT: {
        const int size_needed_for_integer = 20;

        char* dest = blob_append_writeable_section(&buf, size_needed_for_integer);
        ice_assert(is_fblob(buf));

        int charsWritten = sprintf(dest, "%d", suffix.i);

        buf = blob_resize_rel(buf, charsWritten - size_needed_for_integer);
        return buf;
    }
        

    case TAG_FLOAT:
        buf = blob_append_s(buf, "todo");
        return buf;

    case TAG_EX:
        switch (suffix.extag) {
        case EX_TAG_NIL:
            buf = blob_append_s(buf, "nil");
            return buf;

        case EX_TAG_EMPTY_LIST:
            buf = blob_append_s(buf, "[]");
            return buf;

        case EX_TAG_EMPTY_TABLE:
            buf = blob_append_s(buf, "{}");
            return buf;

        case EX_TAG_EMPTY_BLOB:
            buf = blob_append_s(buf, "''");
            return buf;

        case EX_TAG_TRUE:
            buf = blob_append_s(buf, "true");
            return buf;

        case EX_TAG_FALSE:
            buf = blob_append_s(buf, "false");
            return buf;

        }
        break;
    }

    internal_error("unhandled type in stringify_append");
    return buf;
}

Value stringify(Value value)
{
    return stringify_append(empty_blob(), value);
}

void print_raw(Value value)
{
    switch (value.tag) {
    case TAG_POINTER:
        switch (value.object_ptr->type) {

        case TYPE_ARRAY:
            printf("array{rc = %d, length = %d, cap = %d, items = [",
                value.array_ptr->refcount, value.array_ptr->length, value.array_ptr->capacity);
            for (int i=0; i < value.array_ptr->length; i++) {
                if (i > 0)
                    printf(", ");
                print_raw(value.array_ptr->items[i]);
            }
            printf("]}");
            return;

        case TYPE_ARRAY_NODE: {
            ArrayNode* node = value.array_node_ptr;
            printf("array_node{rc = %d, length = %d",
                node->refcount, node->length);

            printf(", left = ");
            print_raw(node->left);
            printf(", right = ");
            print_raw(node->right);
            printf("}");
            return;
        }

        case TYPE_ARRAY_SLICE: {
            ArraySlice* slice = value.array_slice_ptr;
            printf("array_slice{rc = %d, start_index = %d, length = %d",
                slice->refcount, slice->start_index, slice->length);
            printf(", base = ");
            print_raw(slice->base);
            printf("}");
            return;
        }

        case TYPE_FBLOB:
            printf("fblob{rc = %d}", value.fblob_ptr->refcount);
            return;

        case TYPE_FBLOB_SYMBOL:
            printf("fblob_symbol{rc = %d}", value.fblob_ptr->refcount);
            return;

        case TYPE_HASHTABLE: {
            Hashtable* ht = value.hashtable_ptr;
            printf("hashtable{rc = %d, length = %d, capacity = %d",
                ht->refcount, ht->length, ht->capacity);
            printf(", pairs = [");
            for (int i=0; i < ht->length; i++) {
                HashtablePair* pair = ht->getPair(i);
                if (i != 0)
                    printf(", ");
                printf("(");
                print_raw(pair->key);
                printf(", ");
                print_raw(pair->value);
                printf(")");
            }
            printf("]}");
            return;
        }
        }
        printf("[error: unknown type enum %d]", value.object_ptr->type);
        return;

    case TAG_EX:
        switch (value.extag) {
        case EX_TAG_NIL:
            printf("nil");
            return;
        case EX_TAG_EMPTY_LIST:
            printf("empty_list");
            return;
        case EX_TAG_EMPTY_TABLE:
            printf("empty_hashtable");
            return;
        case EX_TAG_EMPTY_BLOB:
            printf("empty_blob");
            return;
        case EX_TAG_TRUE:
            printf("true");
            return;
        case EX_TAG_FALSE:
            printf("false");
            return;
        }
        printf("[error: unknown ex tag %d]", value.extag);
        return;

    case TAG_INT:
        printf("%d", value.i);
        return;

    case TAG_FLOAT:
        printf("%f", value.f);
        return;
    }

    printf("[error: unknown tag %d]", value.tag);
}

void dump(Value value)
{
    print_raw(value);
    printf("\n");
}

static int compare_rank_based_on_type(Value val)
{
    if (is_bool(val))
        return 1;
    else if (is_int(val))
        return 2;
    else if (is_float(val))
        return 3;
    else
        return 5;
}

int compare(Value left, Value right)
{
    if (equals(left, right))
        return 0;

    // FIXME
    return 0;
}

bool is_leaf_value(Value value)
{
    return is_int(value)
        || is_float(value)
        //|| is_string(value)
        || is_bool(value)
        || is_null(value);
}

bool is_bool(Value value)
{
    return value.raw == ex_value(EX_TAG_TRUE).raw || value.raw == ex_value(EX_TAG_FALSE).raw;
}

bool is_blob(Value value)
{
    return is_empty_blob(value) || is_fblob(value);
}

bool is_int(Value value)
{
    return value.tag == TAG_INT;
}

bool is_float(Value value)
{
    return value.tag == TAG_FLOAT;
}

bool is_empty_list(Value value)
{
    return is_ex_tag(value) && value.extag == EX_TAG_EMPTY_LIST;
}

bool is_empty_table(Value value)
{
    return is_ex_tag(value) && value.extag == EX_TAG_EMPTY_TABLE;
}

bool is_empty_blob(Value value)
{
    return is_ex_tag(value) && value.extag == EX_TAG_EMPTY_BLOB;
}

bool is_list(Value value)
{
    return is_empty_list(value) || is_array(value) || is_array_node(value) || is_array_slice(value);
}

bool is_symbol(Value value)
{
    return is_fblob_symbol(value);
}

bool is_table(Value value)
{
    return is_empty_table(value) || is_hashtable(value);
}

bool is_null(Value value)
{
    return value.raw == ex_value(EX_TAG_NIL).raw;
}

bool is_hashtable(Value value)
{
    return is_object(value) && value.object_ptr->type == TYPE_HASHTABLE;
}

bool is_number(Value value)
{
    return is_int(value) || is_float(value);
}

float to_float(Value value)
{
    if (is_int(value))
        return (float) value.i;
    else if (is_float(value))
        return value.f;

    internal_error("In to_float, type is not an int or float");
    return 0.0;
}

int to_int(Value value)
{
    if (is_int(value))
        return value.i;
    else if (is_float(value))
        return (int) value.f;

    internal_error("In to_float, type is not an int or float");
    return 0;
}

Value blob_s(const char* str)
{
    u32 size = strlen(str);
    return ptr_value(fblob_fill(new_fblob(size), str, size));
}

Value blob_p(void* ptr)
{
    const u64 size = sizeof(ptr);
    return ptr_value(fblob_fill(new_fblob(size), (char*) &ptr, size));
}

void* as_pointer(Value blob)
{
    const u64 size = sizeof(void*);
    if (!is_blob(blob) || blob_size(blob) != size)
        return NULL;

    #if INTPTR_MAX == INT64_MAX
        // 64-bit
        return (void*) blob_read_u64(blob, 0);
    #elif INTPTR_MAX == INT32_MAX
        // 32-bit
        return (void*) blob_read_u32(blob, 0);
    #endif

    ice_assert(false);
    return NULL;
}

Value new_blob(u32 size)
{
    if (size == 0)
        return empty_blob();

    return ptr_value(new_fblob(size));
}

Value symbol(const char* str)
{
    Value val = blob_s(str);
    ice_assert(is_fblob(val));
    val.fblob_ptr->object_type = TYPE_FBLOB_SYMBOL;
    return val;
}

Value to_symbol(Value value)
{
    if (is_symbol(value))
        return value;

    if (is_fblob(value)) {
        if (!object_is_writeable(value))
            value = blob_touch(value);

        value.fblob_ptr->object_type = TYPE_FBLOB_SYMBOL;
        return value;
    }

    ice_assert(false);
    return nil_value();
}

Value int_value(i32 i)
{
    Value v;
    v.i = i;
    v.padding_i = 0;
    v.tag = TAG_INT;
    return v;
}

Value float_value(f32 f)
{
    Value v;
    v.f = f;
    v.padding_f = 0;
    v.tag = TAG_FLOAT;
    return v;
}

Value bool_value(bool b)
{
    if (b)
        return true_value();
    else
        return false_value();
}

Value ptr_value(void* ptr)
{
    ice_assert(ptr != NULL);

    Value v;

    // Zero out the entire value, in case we have 32-bit pointers.
    v.raw = 0;

    v.ptr = (ObjectHeader*) ptr;
    ice_assert(v.tag == 0);
    return v;
}

Value nullable_ptr_value(void* ptr)
{
    if (ptr == NULL)
        return nil_value();
    else
        return ptr_value(ptr);
}

Value ex_value(u8 extag)
{
    Value v;
    v.extag = extag;
    v.padding_ex = 0;
    v.tag = TAG_EX;
    return v;
}

Value nil_value() { return ex_value(EX_TAG_NIL); }
Value true_value() { return ex_value(EX_TAG_TRUE); }
Value false_value() { return ex_value(EX_TAG_FALSE); }
Value empty_list() { return ex_value(EX_TAG_EMPTY_LIST); }
Value empty_table() { return ex_value(EX_TAG_EMPTY_TABLE); }
Value empty_blob() { return ex_value(EX_TAG_EMPTY_BLOB); }

u32 length(Value list)
{
    if (is_array(list) || is_array_node(list) || is_array_slice(list))
        return list.array_ptr->length;

    if (is_hashtable(list))
        return list.hashtable_ptr->length;

    return 0;
}

Value get_index(Value list, int index)
{
    if (index < 0)
        return nil_value();

    switch (list.tag) {
    case TAG_POINTER:
        switch (list.object_ptr->type) {
        case TYPE_ARRAY: {
            Array* array = list.array_ptr;
            if (index >= array->length)
                return nil_value();
            return array->items[index];
        }
        case TYPE_ARRAY_SLICE: {
            ArraySlice* slice = list.array_slice_ptr;
            return get_index(slice->base, index + slice->start_index);
        }
        case TYPE_ARRAY_NODE: {
            ArrayNode* node = list.array_node_ptr;
            int left_len = length(node->left);
            if (index < left_len)
                return get_index(node->left, index);
            else
                return get_index(node->right, index - left_len);
        }
        case TYPE_HASHTABLE: {
            Hashtable* ht = list.hashtable_ptr;
            if (index >= ht->length)
                return nil_value();
            return ht->getPair(index)->value;
        }
        }
    }
    return nil_value();
}

void Value::dump()
{
    ::dump(*this);
}

CValue::CValue()
{
    init_value(this);
}

CValue::~CValue()
{
    decref(*this);
}

void CValue::dump()
{
    Value as_str = stringify(*this);
    blob_print(as_str);
    printf("\n");
    decref(as_str);
}

} // namespace ice
