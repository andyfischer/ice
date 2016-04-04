
#include "ice_internal_headers.h"

#include "value.h"
#include "iterator.h"
#include "block.h"

#define min(x,y) ((x) < (y) ? (x) : (y))

Flat* new_flat(u8 logical_type, u16 size)
{
    Flat* flat = malloc(sizeof(Flat) + size);
    flat->header.block_type = FLAT_BLOCK;
    flat->header.logical_type = logical_type;
    flat->header.refcount = 1;
    flat->header.size = size;
    return flat;
}

Slice* new_slice(u8 logical_type, u16 start_pos, u16 size, Value base)
{
    Slice* slice = (Slice*) malloc(sizeof(Slice));
    slice->header.block_type = SLICE_BLOCK;
    slice->header.logical_type = logical_type;
    slice->header.refcount = 1;
    slice->header.size = size;
    slice->start_pos = start_pos;
    slice->base = base;
    return slice;
}

Node* new_node(u8 logical_type, Value left, Value right)
{
    assert(refcount(left) > 0);
    assert(refcount(right) > 0);

    Node* node = (Node*) malloc(sizeof(Node));
    node->header.block_type = NODE_BLOCK;
    node->header.logical_type = logical_type;
    node->header.refcount = 1;
    node->header.size = block_size(left) + block_size(right);
    node->left = left;
    node->right = right;
    return node;
}

bool is_flat_block(Value value)
{
    return is_object(value) && value.object->block_type == FLAT_BLOCK;
}

bool is_slice_block(Value value)
{
    return is_object(value) && value.object->block_type == SLICE_BLOCK;
}

bool is_node_block(Value value)
{
    return is_object(value) && value.object->block_type == NODE_BLOCK;
}

u16 block_size(Value value)
{
    if (is_object(value))
        return value.object->size;

    return 0;
}

u8* block_get(Value obj, u32 offset)
{
    assert(is_object(obj));
    assert(offset < obj.object->size);
    assert(offset >= 0);

    switch (obj.object->block_type) {
    case FLAT_BLOCK:
        return obj.flat->data + offset;
    case SLICE_BLOCK:
        return block_get(obj.slice->base, offset + obj.slice->start_pos);
    case NODE_BLOCK: {
        u32 left_size = block_size(obj.node->left);
        if (offset < left_size)
            return block_get(obj.node->left, offset);
        else
            return block_get(obj.node->right, offset - left_size);
    }
    default:
        assert(false);
        return NULL;
    }
}

Value prepend(Value list, Value prefix)
{
    if (is_empty_list(list) || !is_object(list))
        return list1(prefix);

    return ptr_value(new_node(list.object->logical_type, list1(prefix), list));
}

u32 length(Value list)
{
    if (is_object(list))
        return block_size(list) / sizeof(Value);
    return 0;
}

Value concat(Value left /*consumed*/, Value right /*consumed*/)
{
    check_value(left);
    check_value(right);

    if (is_empty(left))
        return right;
    if (is_empty(right))
        return left;

    assert(left.raw == right.raw ? refcount(left) >= 2 : 1);

    return ptr_value(new_node(left.object->logical_type, left, right));
}

u8* append_writeable_section(Value* obj, u32 size)
{
    Flat* section = new_flat(get_logical_type(*obj), size);
    *obj = concat(*obj, ptr_value(section));
    return section->data;
}

Value append_u8(Value obj /*consumed*/, u8 val)
{
    u8* dest = append_writeable_section(&obj, 1);
    *dest = val;
    return obj;
}

Value append_bytes_len(Value obj, const u8* bytes, u32 size)
{
    u8* dest = append_writeable_section(&obj, size);
    memcpy(dest, bytes, size);
    return obj;
}

Value append_str_len(Value obj, const char* str, u32 size)
{
    u8* dest = append_writeable_section(&obj, size);
    memcpy(dest, str, size);
    return obj;
}

Value append_str(Value obj, const char* str)
{
    size_t size = strlen(str);
    u8* dest = append_writeable_section(&obj, size);
    memcpy(dest, str, size);
    return obj;
}

Value byte_slice(Value base, u32 start_offset, u32 size)
{
    // Simplify slice-of-slice
    
    if (is_slice_block(base))
        return byte_slice(base.slice->base, base.slice->start_pos + start_offset,
                min(size, base.slice->header.size));

    return ptr_value(new_slice(get_logical_type(base), start_offset, size, base));
}

// Slice with logical offset (if it's a list type, then an offset of 1 is the size of one Value)
Value slice(Value base, u32 start_offset, u32 size)
{
    if (get_logical_type(base) == LIST_TYPE) {
        u32 actual_start_offset = start_offset * sizeof(Value);
        u32 actual_size = size * sizeof(Value);
        return byte_slice(base, actual_start_offset, actual_size);
    }

    return byte_slice(base, start_offset, size);
}

Value flatten(Value val)
{
    if (!is_object(val))
        return val;
    if (val.object->block_type == FLAT_BLOCK)
        return val;

    Flat* flat = new_flat(val.object->logical_type, val.object->size);

    size_t dest_offset = 0;
    for_each_section(val, it) {
        u32 source_size;
        u8* source = iterator_get_section(&it, &source_size);
        memcpy(flat->data + dest_offset, source, source_size);
        dest_offset += source_size;
    }

    return ptr_value(flat);
}
