
#include "ice_internal_headers.h"

#include "value.h"
#include "block.h"
#include "iterator.h"

#define SAFE_ITERATOR 1

#define min(x,y) ((x) < (y) ? (x) : (y))

Iterator iterator_start(Value obj)
{
    if (is_empty_blob(obj) || is_empty_list(obj)) {
        Iterator it;
        it.object = nil_value();
        it.offset = 0;
        it.end_pos = 0;
        it.stack = nil_value();
        return it;
    }

    Iterator it;
    it.object = obj;
    it.offset = 0;
    it.end_pos = block_size(obj);
    it.stack = nil_value();

    #if SAFE_ITERATOR
        incref(obj);
    #endif

    if (is_object(obj)) {
        switch (obj.object->block_type) {
        case FLAT_BLOCK:
            break;
        case SLICE_BLOCK:
        case NODE_BLOCK:
            iterator_settle(&it);
            break;
        }
    }



    return it;
}

bool iterator_done(Iterator* it)
{
    return is_nil(it->object);
}

void iterator_advance(Iterator* it, u16 dist)
{
    it->offset += dist;
    iterator_settle(it);
}

void iterator_advance_val(Iterator* it)
{
    it->offset += sizeof(Value);
    iterator_settle(it);
}

u8 iterator_get_u8(Iterator* it)
{
    assert(!iterator_done(it));
    assert(is_flat_block(it->object));
    return it->object.flat->data[it->offset];
}

Value iterator_get_val(Iterator* it)
{
    assert(!iterator_done(it));
    assert(is_flat_block(it->object));
    return *((Value*) (it->object.flat->data + it->offset));
}

u8* iterator_get_section(Iterator* it, u32* size)
{
    assert(!iterator_done(it));
    assert(is_flat_block(it->object));

    *size = it->end_pos - it->offset;
    return it->object.flat->data + it->offset;
}

void iterator_advance_section(Iterator* it)
{
    it->offset = it->end_pos;
    iterator_settle(it);
}

void iterator_pop_stack(Iterator* it)
{
    assert(!is_nil(it->stack));
    Value stack = it->stack;

    #if SAFE_ITERATOR
        decref(it->object);
    #endif
    it->object = take_nth(stack, 0);
    it->offset = nth(stack, 1).i;
    it->end_pos = nth(stack, 2).i;
    it->stack = take_nth(stack, 3);

    #if SAFE_ITERATOR
        decref(stack);
    #else
        free(stack.flat);
    #endif
}

void iterator_stop(Iterator* it)
{
    while (!is_nil(it->stack))
        iterator_pop_stack(it);

    #if SAFE_ITERATOR
        decref(it->object);
    #endif

    it->object = nil_value();
}


// Returns true if the settle is done
bool iterator_settle_one_step(Iterator* it)
{
    if (!is_object(it->object)) {
        it->object = nil_value();
        return true;
    }

    switch (it->object.object->block_type) {
    case FLAT_BLOCK: {
        Flat* flat = it->object.flat;

        if (it->offset >= it->end_pos) {
            if (is_nil(it->stack)) {
                // Done
                iterator_stop(it);
                return true;
            }
            // Stack is not empty, pop stack frame and keep settling.
            iterator_pop_stack(it);
            return false;
        }

        return true;
    }

    case SLICE_BLOCK: {
        // Jump into the sliced block
        Slice* slice = it->object.slice;
        u32 remaining_size = min(it->end_pos - it->offset, slice->header.size);
        it->offset += slice->start_pos;
        it->end_pos = it->offset + remaining_size;

        #if SAFE_ITERATOR
            incref(slice->base);
            decref(it->object);
        #endif

        it->object = slice->base;
        return false; // not done
    }

    case NODE_BLOCK: {
        Node* node = it->object.node;
        u16 left_size = block_size(node->left);

        if (it->offset >= block_size(node->left)) {
            // Skip left side altogether, jump into right side
            #if SAFE_ITERATOR
                incref(node->right);
                decref(it->object);
            #endif
            it->object = node->right;
            it->offset -= left_size;
            it->end_pos -= left_size;
            return false; // not done
        }

        if (it->end_pos < left_size) {
            // Enter left side and don't push to stack, because the end pos is within the left side.
            #if SAFE_ITERATOR
                incref(node->left);
                decref(it->object);
            #endif
            it->object = node->left;
            return false; // not done
        }

        // Enter left side and push right to stack for later

        #if SAFE_ITERATOR
            incref(node->left);
            incref(node->right);
            decref(it->object);
        #endif

        it->stack = list4(node->right, int_value(0), int_value(it->end_pos - left_size), it->stack);
        assert(is_flat_block(it->stack));
        it->end_pos = left_size;
        it->object = node->left;
        return false; // not done
    }
    }
    return true;
}

void iterator_settle(Iterator* it)
{
    while (!iterator_settle_one_step(it));
}

