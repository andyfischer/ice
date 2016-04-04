
#include "ice_internal_headers.h"

#include "blob.h"
#include "block.h"
#include "list.h"
#include "symbol.h"
#include "table.h"
#include "value.h"

void nullify(Value* value)
{
    decref(*value);
    *value = nil_value();
}

Value incref(Value value)
{
    stat_inc(stat_incref);

    if (!is_object(value))
        return value;

    assert(value.object->refcount > 0);

    if (value.object->refcount != REFCOUNT_PERM)
        value.object->refcount++;

    return value;
}

void decref(Value value)
{
    check_value(value);
    stat_inc(stat_decref);

    if (!is_object(value))
        return;

    if (value.object->refcount == REFCOUNT_PERM)
        return;

    assert(value.object->refcount > 0);

    if (value.object->refcount == 1) {
        #if DEBUG
            value.object->refcount = 0;
        #endif

        switch (value.object->block_type) {
        case FLAT_BLOCK: {
            Flat* flat = value.flat;
            if (flat->header.logical_type == LIST_TYPE) {
                assert((flat->header.size % sizeof(Value)) == 0);
                for (u32 pos=0; pos < flat->header.size; pos += sizeof(Value)) {
                    Value el = *(Value*)(flat->data + pos);
                    decref(el);
                }
            }
            free(flat);
            return;
        }
        case SLICE_BLOCK:
            decref(value.slice->base);
            free(value.slice);
            return;
        case NODE_BLOCK:
            decref(value.node->left);
            decref(value.node->right);
            free(value.node);
            return;
        }
        return;
    }

    value.object->refcount--;
}

void decref2(Value value1, Value value2)
{
    decref(value1);
    decref(value2);
}

void decref3(Value value1, Value value2, Value value3)
{
    decref(value1);
    decref(value2);
    decref(value3);
}

void decref4(Value value1, Value value2, Value value3, Value value4)
{
    decref(value1);
    decref(value2);
    decref(value3);
    decref(value4);
}

void decref5(Value value1, Value value2, Value value3, Value value4, Value value5)
{
    decref(value1);
    decref(value2);
    decref(value3);
    decref(value4);
    decref(value5);
}

u8 get_logical_type(Value value)
{
    if (is_object(value))
        return value.object->logical_type;

    if (is_empty_list(value))
        return LIST_TYPE;

    if (is_empty_blob(value))
        return BLOB_TYPE;

    if (is_int(value))
        return INT_TYPE;

    return 0;
}

bool is_object(Value value)
{
    return value.tag == 0 && value.object != NULL;
}

Value set_logical_type(Value value, u8 logical_type)
{
    assert(logical_type != LIST_TYPE);
    assert(get_logical_type(value) != LIST_TYPE);

    if (is_object(value)) {
        if (refcount(value) == 1) {
            value.object->logical_type = logical_type;
            return value;
        }
        
        return ptr_value(new_slice(logical_type, 0, block_size(value), value));
    }

    assert(false);
    return nil_value();
}

Value make_perm(Value value)
{
    if (is_object(value))
        value.object->refcount = REFCOUNT_PERM;
    return value;
}

void free_perm(Value value)
{
    if (is_object(value)) {
        value.object->refcount = 1;
        decref(value);
    }
}

bool object_is_writeable(Value value)
{
    assert(is_object(value));
    return value.object->refcount == 1;
}

bool is_writeable_object(Value value)
{
    return is_object(value) && value.object->refcount == 1;
}

int refcount(Value value)
{
    if (!is_object(value))
        return 1;
    return value.object->refcount;
}

bool is_ex_tag(Value value)
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
    //TODO
    return false;
}

bool equals(Value left, Value right)
{
    if (shallow_equals(left, right))
        return true;

    u8 logical_type_left = get_logical_type(left);
    u8 logical_type_right = get_logical_type(right);

    if (logical_type_left != logical_type_right)
        return false;

    switch (logical_type_left) {
    case INT_TYPE:
        return left.i == right.i;
    
    case LIST_TYPE: {
        Iterator left_it = iterator_start(left);
        Iterator right_it = iterator_start(right);
        bool result = true;

        while (!iterator_done(&left_it) && !iterator_done(&right_it)) {

            if (!equals(iterator_get_val(&left_it), iterator_get_val(&right_it))) {
                result = false;
                break;
            }

            iterator_advance_val(&left_it);
            iterator_advance_val(&right_it);
        }

        if (!iterator_done(&left_it)) {
            iterator_stop(&left_it);
            result = false;
        }

        if (!iterator_done(&right_it)) {
            iterator_stop(&right_it);
            result = false;
        }
            
        return result;
    }

    case BLOB_TYPE:
    case SYMBOL_TYPE: {
        Iterator left_it = iterator_start(left);
        Iterator right_it = iterator_start(right);
        bool result = true;

        while (!iterator_done(&left_it) && !iterator_done(&right_it)) {

            if (iterator_get_u8(&left_it) != iterator_get_u8(&right_it)) {
                result = false;
                break;
            }

            iterator_advance(&left_it, 1);
            iterator_advance(&right_it, 1);
        }

        if (!iterator_done(&left_it)) {
            iterator_stop(&left_it);
            result = false;
        }

        if (!iterator_done(&right_it)) {
            iterator_stop(&right_it);
            result = false;
        }
            
        return result;
    }
    }

    assert(false);
    return false;
}

bool equals_str(Value lhs, const char* str)
{
    if (!is_blob(lhs))
        return false;
    Value rhsStr = from_str(str);
    bool result = equals(lhs, rhsStr);
    decref(rhsStr);
    return result;
}

Value deep_replace(Value obj /*consumed*/, Value target, Value replacement)
{
    if (equals(obj, target)) {
        decref(obj);
        return incref(replacement);
    }

    if (is_list(obj)) {
        for (int i=0; i < length(obj); i++) {
            if (equals(nth(obj, i), target))
                obj = set_nth(obj, i, incref(replacement));
        }
    } else if (is_table(obj)) {

        for (int i=0; i < length(obj); i++) {
            Value key = nth(obj, i);
            if (equals(key, target)) {
                obj = delete_key(obj, key);
                obj = insert(obj, incref(target), incref(replacement));
            }

            Value val = nth(obj, i);
            if (equals(val, target))
                obj = set_nth(obj, i, incref(replacement));
        }
    }

    return obj;
}

u32 hashcode_raw(u8* data, size_t size)
{
    u32 result = 0;
    int offset = 0;
    for (size_t i=0; i < size; i++) {
        char c = data[i];
        result ^= c << (offset*3);
        offset = (offset + 1) % 4;
    }
    return result;
}

u32 hashcode(Value val)
{
    u32 result = 0;

    if (is_list(val)) {
        result = (u32) (EX_TAG_EMPTY_LIST << 8);
        u32 len = length(val);
        for (int i=0; i < len; i++)
            result ^= hashcode(nth(val, i));
    } else if (is_blob(val) || is_symbol(val)) {
        int offset = 0;
        for_each_byte(val, it)  {
            char c = iterator_get_u8(&it);
            result ^= c << (offset*3);
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
    if (is_empty_blob(value)) {
        return;
    }

    if (is_blob(value)) {
        for_each_section(value, it) {
            u32 len;
            u8* section = iterator_get_section(&it, &len);
            printf("%.*s", len, section);
        }
        return;
    }

    Value rep;

    rep = stringify(value);
    assert(is_blob(rep));
    print(rep);
    decref(rep);
}

void println(Value value)
{
    print(value);
    printf("\n");
}

Value stringify_append(Value buf /*consumed*/, Value suffix)
{
    assert(is_blob(buf));

    switch (suffix.tag) {
    case TAG_OBJECT:
        assert(suffix.object != NULL);

        switch (suffix.object->logical_type) {

        case LIST_TYPE: {
            bool first = true;
            buf = append_str_len(buf, "[", 1);
            for_each_list_item(suffix, it) {
                if (!first)
                    buf = append_str_len(buf, ", ", 2);
                buf = stringify_append(buf, iterator_get_val(&it));
                first = false;
            }
            buf = append_str_len(buf, "]", 1);
            return buf;
        }

        case BLOB_TYPE:
            buf = append_str_len(buf, "\"", 1);
            buf = concat(buf, incref(suffix));
            buf = append_str_len(buf, "\"", 1);
            return buf;

        case SYMBOL_TYPE:
            buf = append_str_len(buf, ":", 1);
            buf = concat(buf, incref(suffix));
            return buf;
        }

        break;

    case TAG_OPAQUE_POINTER: {
        char str[30];
        snprintf(str, 30, "%p", as_opaque_pointer(suffix));
        return append_str(buf, str);
    }

    case TAG_EX:
        switch (suffix.extag) {
        case EX_TAG_NIL:
            buf = append_str(buf, "nil");
            return buf; {
        }

        case EX_TAG_INT: {
            char str[30];
            snprintf(str, 30, "%d", suffix.i);
            return append_str(buf, str);
        }

        case EX_TAG_FLOAT: {
            char str[30];
            snprintf(str, 30, "%f", suffix.f);
            buf = append_str(buf, str);
            return buf;
        }

        case EX_TAG_EMPTY_LIST:
            buf = append_str(buf, "[]");
            return buf;

        case EX_TAG_EMPTY_TABLE:
            buf = append_str(buf, "{}");
            return buf;

        case EX_TAG_EMPTY_BLOB:
            buf = append_str(buf, "''");
            return buf;

        case EX_TAG_TRUE:
            buf = append_str(buf, "true");
            return buf;

        case EX_TAG_FALSE:
            buf = append_str(buf, "false");
            return buf;

        }
        break;
    }

    printf("error: unhandled type in stringify_append: ");
    dump(suffix);

    return buf;
}

Value stringify(Value value)
{
    return stringify_append(empty_blob(), value);
}

const char* logical_type_name(u8 logical_type)
{
    switch (logical_type) {
    case LIST_TYPE: return "list";
    case BLOB_TYPE: return "blob";
    case SYMBOL_TYPE: return "symbol";
    case TEXT_TYPE: return "text";
    }
    return "";
}

void print_raw(Value value)
{
    switch (value.tag) {
    case TAG_OBJECT:
        if (value.object == NULL) {
            printf("{null}");
            return;
        }

        switch (value.object->block_type) {

        case FLAT_BLOCK: {
            Flat* flat = value.flat;
            printf("flat#%d{%s, rc = %d, size = %d}",
                    managed_alloc_get_id(flat),
                    logical_type_name(flat->header.logical_type),
                    flat->header.refcount, flat->header.size);
            return;
        }

        case SLICE_BLOCK: {
            Slice* slice = value.slice;
            printf("slice#%d{%s, rc = %d, size = %d, start_pos = %d, base = ",
                managed_alloc_get_id(slice),
                logical_type_name(slice->header.logical_type),
                slice->header.refcount, slice->header.size,
                slice->start_pos);
            print_raw(slice->base);
            printf("}");
            return;
        } 
        case NODE_BLOCK: {
            Node* node = value.node;
            printf("node#%d{%s, rc = %d, size = %d, left = ",
                managed_alloc_get_id(node),
                logical_type_name(node->header.logical_type),
                node->header.refcount, node->header.size);
            print_raw(node->left);
            printf(", right = ");
            print_raw(node->right);
            printf("}");
            return;
        }
        }
        printf("[error: unknown block type %d]", value.object->block_type);
        return;

    case TAG_EX:
        switch (value.extag) {
        case EX_TAG_NIL:
            printf("nil");
            return;
        case EX_TAG_INT:
            printf("%d", value.i);
            return;
        case EX_TAG_FLOAT:
            printf("%f", value.f);
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
        || is_nil(value);
}

bool is_bool(Value value)
{
    return value.raw == ex_value(EX_TAG_TRUE).raw || value.raw == ex_value(EX_TAG_FALSE).raw;
}

bool is_int(Value value)
{
    return value.tag == TAG_EX && value.extag == EX_TAG_INT;
}

bool is_float(Value value)
{
    return value.tag == TAG_EX && value.extag == EX_TAG_FLOAT;
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

bool is_empty(Value value)
{
    return is_empty_list(value) || is_empty_table(value) || is_empty_blob(value);
}

bool is_blob(Value value)
{
    return is_empty_blob(value) || (is_object(value) && value.object->logical_type == BLOB_TYPE);
}

bool is_list(Value value)
{
    return is_empty_list(value) || (is_object(value) && value.object->logical_type == LIST_TYPE);
}

bool is_table(Value value)
{
    return is_empty_table(value) || is_hashtable(value);
}

bool is_truthy(Value value) { return !(value.raw == false_value().raw || is_nil(value)); }
bool is_opaque_pointer(Value value) { return value.tag == TAG_OPAQUE_POINTER; }

bool is_nil(Value value)
{
    return value.raw == ex_value(EX_TAG_NIL).raw;
}

bool is_hashtable(Value value)
{
    return is_object(value) && value.object->logical_type == TABLE_TYPE;
}

bool is_number(Value value)
{
    return is_int(value) || is_float(value);
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

Value int_value(i32 i)
{
    Value v;
    v.i = i;
    v.padding_i = 0;
    v.extag = EX_TAG_INT;
    v.tag = TAG_EX;
    return v;
}

Value float_value(f32 f)
{
    Value v;
    v.f = f;
    v.padding_f = 0;
    v.extag = EX_TAG_FLOAT;
    v.tag = TAG_EX;
    return v;
}

Value bool_value(bool b)
{
    if (b)
        return true_value();
    else
        return false_value();
}

Value ptr_value(void* object)
{
    assert(object != NULL);

    Value v = {.raw = 0};

    v.object = (ObjectHeader*) object;
    assert(v.tag == 0);
    return v;
}

Value opaque_ptr(void* ptr)
{
    Value v = {.raw = 0};
    v.ptr = ptr;
    v.tag = TAG_OPAQUE_POINTER;
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
    v.padding_ex_1 = 0;
    v.padding_ex_2 = 0;
    v.extag = extag;
    v.tag = TAG_EX;
    return v;
}

Value nil_value() { return ex_value(EX_TAG_NIL); }
Value true_value() { return ex_value(EX_TAG_TRUE); }
Value false_value() { return ex_value(EX_TAG_FALSE); }
Value empty_list() { return ex_value(EX_TAG_EMPTY_LIST); }
Value list0() { return ex_value(EX_TAG_EMPTY_LIST); }
Value empty_table() { return ex_value(EX_TAG_EMPTY_TABLE); }
Value empty_blob() { return ex_value(EX_TAG_EMPTY_BLOB); }

void* as_opaque_pointer(Value p)
{
    p.tag = 0;
    return p.ptr;
}

Value nth(Value list, int index)
{
    if (index < 0)
        return nil_value();

    if (is_object(list)) {
        switch (list.object->logical_type) {
        case LIST_TYPE:
            return *((Value*) block_get(list, sizeof(Value) * index));
        case TABLE_TYPE:
            assert(false);
        default:
            return nil_value();
        }
    }

    return nil_value();
}

Value* nth_addr(Value list, int index)
{
    if (index < 0)
        return NULL;

    assert(false);
    return NULL;
}

Value get(Value obj, Value key)
{
    if (is_list(obj)) {
        if (!is_int(key))
            return nil_value();

        return nth(obj, key.i);
    }

    if (is_empty_table(obj))
        return nil_value();

    if (is_hashtable(obj)) {
        assert(false);
    }

    return nil_value();
}

Value get2(Value obj, Value key1, Value key2)
{
    return get(get(obj, key1), key2);
}

Value* get_addr(Value obj, Value key)
{
    if (is_list(obj)) {
        if (!is_int(key))
            return NULL;

        return nth_addr(obj, key.i);
    }

    if (is_empty_table(obj))
        return NULL;

    if (is_hashtable(obj)) {
        assert(false);
    }

    return NULL;
}

Value set_nth(Value obj, int index, Value el)
{
    check_value(obj);
    check_value(el);

    if (!is_list(obj) || is_empty_list(obj))
        return obj;

    assert(is_object(obj));
    if (index < 0 || index >= length(obj)) {
        decref(el);
        return obj;
    }

    if (refcount(obj) == 1 && is_flat_block(obj)) {
        Value* dest = ((Value*) obj.flat->data) + index;
        decref(*dest);
        *dest = el;
        check_value(*dest);
        return obj;
    }

    assert(false);
    return nil_value();
}

Value apply_nth(Value list, int index, func_1 func)
{
    return set_nth(list, index, func(take_nth(list, index)));
}

Value set(Value obj, Value key, Value el)
{
    if (is_table(obj))
        return insert(obj, key, el);

    if (is_list(obj)) {
        if (!is_int(key))
            return obj;

        return set_nth(obj, key.i, el);
    }

    return obj;
}

Value set_path_a(Value obj /*consumed*/, Value* pathArr, int pathLen, Value el /*consumed*/)
{
    if (pathLen == 0)
        return obj;
    if (pathLen == 1)
        return set(obj, pathArr[0], el);

    if (object_is_writeable(obj)) {
        Value* target = get_addr(obj, pathArr[0]);
        if (target != NULL) {
            *target = set_path_a(*target, pathArr + 1, pathLen - 1, el);
            return obj;
        }
    }

    Value target = take(obj, pathArr[0]);
    target = set_path_a(target, pathArr + 1, pathLen - 1, el);
    return set(obj, pathArr[0], el);
}

Value take(Value obj, Value key)
{
    assert(false);
    return nil_value();
}

Value take_value(Value obj, Value key)
{
    assert(false);
    return nil_value();
}

Value append(Value obj, Value suffix)
{
    if (is_empty_list(obj) || !is_object(obj))
        return list1(suffix);

    Value* dest = (Value*) append_writeable_section(&obj, sizeof(Value));
    *dest = suffix;
    return obj;
}
