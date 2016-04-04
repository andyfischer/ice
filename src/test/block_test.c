
#include "ice_internal_headers.h"

#include "test_framework.h"
#include "block.h"
#include "value.h"

Value get_sample_flat(u8 logical_type, u32 size)
{
    Value val = ptr_value(new_flat(logical_type, size));
    for (u8 i=0; i < size; i++)
        val.flat->data[i] = i;
    return val;
}

void test_alloc_flat()
{
    Flat* flat = new_flat(BLOB_TYPE, 16);
    expect(flat->header.logical_type == BLOB_TYPE);
    expect(flat->header.block_type == FLAT_BLOCK);
    expect(flat->header.refcount == 1);
    expect(flat->header.size == 16);

    Value val = ptr_value(flat);
    expect(val.object->logical_type == BLOB_TYPE);
    expect(val.object->block_type == FLAT_BLOCK);
    expect(val.object->size == 16);
    expect(block_size(val) == 16);
    expect(get_logical_type(val) == BLOB_TYPE);
    expect(val.object->refcount == 1);
    expect(is_object(val));
    expect(is_flat_block(val));
    expect(!is_slice_block(val));
    expect(!is_node_block(val));

    decref(val);
}

void test_alloc_slice()
{
    Value base = ptr_value(new_flat(BLOB_TYPE, 16));
    Slice* slice = new_slice(BLOB_TYPE, 0, 16, base);
    expect(slice->header.refcount == 1);
    expect(slice->header.logical_type == BLOB_TYPE);
    expect(slice->header.block_type == SLICE_BLOCK);
    expect(slice->header.size == 16);
    expect(slice->base.raw == base.raw);

    Value val = ptr_value(slice);
    expect(block_size(val) == 16);
    expect(get_logical_type(val) == BLOB_TYPE);
    expect(val.object->logical_type == BLOB_TYPE);
    expect(val.object->block_type == SLICE_BLOCK);
    expect(is_object(val));
    expect(!is_flat_block(val));
    expect(is_slice_block(val));
    expect(!is_node_block(val));
    decref(val);
}

void test_alloc_node()
{
    Value left = ptr_value(new_flat(BLOB_TYPE, 2));
    Value right = ptr_value(new_flat(BLOB_TYPE, 4));
    Node* node = new_node(BLOB_TYPE, left, right);
    expect(node->header.refcount == 1);
    expect(node->header.logical_type == BLOB_TYPE);
    expect(node->header.block_type == NODE_BLOCK);
    expect(node->header.size == 6);
    expect(node->left.raw == left.raw);
    expect(node->right.raw == right.raw);

    Value val = ptr_value(node);
    expect(block_size(val) == 6);
    expect(get_logical_type(val) == BLOB_TYPE);
    expect(val.object->logical_type == BLOB_TYPE);
    expect(val.object->block_type == NODE_BLOCK);
    expect(is_object(val));
    expect(!is_flat_block(val));
    expect(!is_slice_block(val));
    expect(is_node_block(val));
    decref(val);
}

void test_iterator_on_flat()
{
    Value val = get_sample_flat(BLOB_TYPE, 16);

    Iterator it = iterator_start(val);
    for (u8 i=0; i < 16; i++) {
        expect(!iterator_done(&it));
        expect(iterator_get_u8(&it) == i);
        iterator_advance(&it, 1);
    }
    expect(iterator_done(&it));
    decref(val);
}

void test_iterator_on_slice()
{
    Value val = ptr_value(new_slice(BLOB_TYPE, 4, 8, get_sample_flat(BLOB_TYPE, 16)));

    Iterator it = iterator_start(val);
    for (u8 i=4; i < 12; i++) {
        expect(!iterator_done(&it));
        expect(iterator_get_u8(&it) == i);
        iterator_advance(&it, 1);
    }
    expect(iterator_done(&it));

    // Slice of slice
    Value val2 = ptr_value(new_slice(BLOB_TYPE, 2, 4, val));
    it = iterator_start(val2);
    for (u8 i=6; i < 10; i++) {
        expect(!iterator_done(&it));
        expect(iterator_get_u8(&it) == i);
        iterator_advance(&it, 1);
    }
    expect(iterator_done(&it));

    decref(val2);
}

void test_iterator_on_node()
{
    Value left = get_sample_flat(BLOB_TYPE, 8);
    Value right = get_sample_flat(BLOB_TYPE, 8);
    Value val = ptr_value(new_node(BLOB_TYPE, left, right));

#if 0
    Iterator it;
    iterator_start(&it, val);
    for (u8 i=0; i < 16; i++) {
        expect(!iterator_done(&it));
        expect(iterator_get_u8(&it) == i);
        iterator_advance(&it, 1);
    }
    expect(iterator_done(&it));
#endif

    decref(val);
}

void test_iterator_on_empty()
{
    Value obj;
    Iterator it;

    obj = empty_list();
    it = iterator_start(obj);
    expect(iterator_done(&it));

    obj = empty_blob();
    it = iterator_start(obj);
    expect(iterator_done(&it));
}

void test_iteration_by_section()
{
    Value val = get_sample_flat(BLOB_TYPE, 8);

    Iterator it = iterator_start(val);

    u32 bufSize;
    u8* nextBuf = iterator_get_section(&it, &bufSize);
    expect(nextBuf[0] == 0);
    expect(nextBuf[7] == 7);
    expect(bufSize == 8);
    expect(!iterator_done(&it));
    iterator_advance_section(&it);
    expect(iterator_done(&it));

    Value slice = ptr_value(new_slice(BLOB_TYPE, 2, 4, val));

    it = iterator_start(slice);
    nextBuf = iterator_get_section(&it, &bufSize);
    expect(bufSize == 4);
    expect(nextBuf[0] == 2);
    expect(nextBuf[3] == 5);
    expect(!iterator_done(&it));
    iterator_advance_section(&it);
    expect(iterator_done(&it));

    decref(slice);
}

void test_flatten()
{
    Value v = get_sample_flat(BLOB_TYPE, 8);
    expect(flatten(v).raw == v.raw);
    decref(v);
}

void test_block_get()
{
    Value v = get_sample_flat(BLOB_TYPE, 8);
    expect(*block_get(v, 0) == 0);
    expect(*block_get(v, 1) == 1);
    expect(*block_get(v, 4) == 4);

    v = ptr_value(new_slice(BLOB_TYPE, 2, 4, v));
    expect(*block_get(v, 0) == 2);
    expect(*block_get(v, 2) == 4);

    v = ptr_value(new_node(BLOB_TYPE, v, get_sample_flat(BLOB_TYPE, 4)));
    expect(*block_get(v, 0) == 2);
    expect(*block_get(v, 2) == 4);
    expect(*block_get(v, 4) == 0);
    expect(*block_get(v, 6) == 2);

    decref(v);
}

void block_test()
{
    test_case(test_alloc_flat);
    test_case(test_alloc_slice);
    test_case(test_alloc_node);
    test_case(test_iterator_on_empty);
    test_case(test_iterator_on_flat);
    test_case(test_iterator_on_slice);
    test_case(test_iterator_on_node);
    test_case(test_iteration_by_section);
    test_case(test_flatten);
    test_case(test_block_get);
}

