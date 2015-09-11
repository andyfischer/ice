// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "blob.h"
#include "list.h"
#include "tagged_value.h"

namespace ice {

// Private functions
void array_iterator_stack_push(ArrayIterator* it, Value el);
Value array_iterator_stack_pop(ArrayIterator* it);
void array_iterator_settle(ArrayIterator* it);

Array* new_array(int length, int capacity)
{
    ice_assert(length <= capacity);

    Array* array = (Array*) ice_malloc(sizeof(Array) + capacity * sizeof(Value));
    array->object_type = TYPE_ARRAY;
    array->refcount = 1;
    array->length = length;
    array->capacity = capacity;

    for (int i=0; i < capacity; i++)
        init_value(&array->items[i]);

    return array;
}

Array* new_array(int length)
{
    return new_array(length, length);
}

ArrayNode* new_array_node(Value left /*consumed*/, Value right /*consumed*/)
{
    ice_assert(is_list(left));
    ice_assert(is_list(right));

    ArrayNode* array = (ArrayNode*) ice_malloc(sizeof(ArrayNode));
    array->object_type = TYPE_ARRAY_NODE;
    array->refcount = 1;
    array->left = left;
    array->right = right;
    array->length = length(left) + length(right);

    return array;
}

ArraySlice* new_array_slice(Value list_consumed, int start_index, int length)
{
    ArraySlice* slice = (ArraySlice*) ice_malloc(sizeof(ArraySlice));
    slice->object_type = TYPE_ARRAY_SLICE;
    slice->refcount = 1;
    slice->length = length;
    slice->start_index = start_index;
    slice->base = list_consumed;
    return slice;
}

void free_array(Array* array)
{
    for (int i=0; i < array->length; i++)
        decref(array->items[i]);

    free(array);
}

void free_array_node(ArrayNode* node)
{
    decref(node->left);
    decref(node->right);
    free(node);
}

void free_array_slice(ArraySlice* node)
{
    decref(node->base);
    free(node);
}

bool is_array(Value value)
{
    return (is_object(value) && value.object_ptr->type == TYPE_ARRAY);
}

bool is_array_node(Value value)
{
    return (is_object(value) && value.object_ptr->type == TYPE_ARRAY_NODE);
}

bool is_array_slice(Value value)
{
    return (is_object(value) && value.object_ptr->type == TYPE_ARRAY_SLICE);
}

inline bool is_writeable(Array* array)
{
    return array->refcount == 1;
}

ArrayIterator::ArrayIterator(Value list) { array_iterator_start(this, list); }
Value ArrayIterator::current() { return array_iterator_current(this); }
bool ArrayIterator::valid() { return array_iterator_valid(this); }
void ArrayIterator::advance() { array_iterator_advance(this); }

void array_iterator_start(ArrayIterator* it, Value list)
{
    if (length(list) == 0) {
        it->list = nil_value();
        return;
    }

    it->list = list;
    it->index = 0;
    it->stack = NULL;

    array_iterator_settle(it);
}

void array_iterator_stop(ArrayIterator* it)
{
    it->list = nil_value();
    free(it->stack);
    it->stack = NULL;
}

void array_iterator_settle(ArrayIterator* it)
{
    while (1) {
        if (is_null(it->list)) {
            free(it->stack);
            it->stack = NULL;
            break;
        }

        if (is_array(it->list)) {
            Array* array = it->list.array_ptr;
            if (it->index >= array->length) {
                it->list = array_iterator_stack_pop(it);
                it->index = 0;
                continue;
            }
            break;
        }

        if (is_empty_list(it->list)) {
            it->list = array_iterator_stack_pop(it);
            it->index = 0;
            continue;
        }

        if (is_array_slice(it->list)) {
            if (it->index >= it->list.array_slice_ptr->length) {
                it->list = array_iterator_stack_pop(it);
                it->index = 0;
                continue;
            }
            break;
        }

        {
            ice_assert(it->list.object_ptr->type == TYPE_ARRAY_NODE);
            ArrayNode* node = it->list.array_node_ptr;

            // If 'left' is non-empty, then search that list first, and push 'right' to
            // the stack for later.
            if (length(node->left) > 0) {
                array_iterator_stack_push(it, node->right);
                it->list = node->left;
                it->index = 0;
                continue;
            } else {
                // Otherwise, search 'right' immediately.
                it->list = node->right;
                it->index = 0;
                continue;
            }

            break;
        }
    }
}

void array_iterator_stack_push(ArrayIterator* it, Value el)
{
    if (it->stack == NULL || it->stack->length >= ARRAY_ITERATOR_STACK_NODE_SIZE) {
        ArrayIteratorStack* top = (ArrayIteratorStack*) ice_malloc(sizeof(ArrayIteratorStack));
        top->length = 1;
        top->items[0] = el;
        top->prev = it->stack;
        it->stack = top;
        return;
    }

    it->stack->items[it->stack->length] = el;
    it->stack->length++;
}

Value array_iterator_stack_pop(ArrayIterator* it)
{
    while (1) {
        if (it->stack == NULL)
            return nil_value();
        
        if (it->stack->length == 0) {
            ArrayIteratorStack* top = it->stack;
            it->stack = top->prev;
            free(top);
            continue;
        }

        Value el = it->stack->items[it->stack->length - 1];
        it->stack->length--;

        return el;
    }
}

bool array_iterator_valid(ArrayIterator* it)
{
    return !is_null(it->list);
}

void array_iterator_advance(ArrayIterator* it)
{
    it->index++;
    array_iterator_settle(it);
}

Value array_iterator_current(ArrayIterator* it)
{
    ice_assert(array_iterator_valid(it)); // caller error
    return get_index(it->list, it->index);
}

#if 0
static void mergesort_step(Value* list, SortCompareFunc func, void* context)
{
    int length = list_length(list);
    int middle = int(length / 2);
    int leftLength = middle;
    int rightLength = length - middle;

    if (length <= 1 || leftLength <= 0 || rightLength <= 0)
        return;

    Value left;
    set_list(&left, leftLength);
    Value right;
    set_list(&right, rightLength);

    list_touch(list);

    // Divide

    for (int i=0; i < leftLength; i++)
        move(list_get(list, i), list_get(&left, i));
    for (int i=0; i < rightLength; i++)
        move(list_get(list, i + middle), list_get(&right, i));

    mergesort_step(&left, func, context);
    mergesort_step(&right, func, context);

    // Merge
    int leftIndex = 0;
    int rightIndex = 0;
    int destIndex = 0;

    while (1) {

        if (leftIndex >= list_length(&left)) {
            while (rightIndex < list_length(&right))
                move(list_get(&right, rightIndex++), list_get(list, destIndex++));
            return;
        }

        if (rightIndex >= list_length(&right)) {
            while (leftIndex < list_length(&left))
                move(list_get(&left, leftIndex++), list_get(list, destIndex++));
            return;
        }

        Value* leftValue = list_get(&left, leftIndex);
        Value* rightValue = list_get(&right, rightIndex);
        Value* dest = list_get(list, destIndex++);

        int compareResult = func(context, leftValue, rightValue);
        if (compareResult < 0) {
            move(leftValue, dest);
            leftIndex++;
        } else {
            move(rightValue, dest);
            rightIndex++;
        }
    }
}

static int default_compare_for_sort(void* context, Value* left, Value* right)
{
    return compare(left, right);
}

void list_sort_mergesort(Value* list, SortCompareFunc func, void* context)
{
    if (func == NULL)
        func = default_compare_for_sort;

    mergesort_step(list, func, context);
}

void list_sort(Value* list, SortCompareFunc func, void* context)
{
    list_sort_mergesort(list, func, context);
}

void list_touch(Value* value)
{
    ice_assert(is_list(value));
    Array* data = (Array*) value->ptr;
    *value = ptr_value(list_touch(data));
}

bool list_touch_is_necessary(Value* value)
{
    Array* data = (Array*) value->ptr;
    return !(data == NULL || data->refcount == 1);
}

Value* list_get(Value* value, int index)
{
    ice_assert(is_list(value));
    return list_get((Array*) value->ptr, index);
}

Value* list_get_from_end(Value* value, int reverseIndex)
{
    ice_assert(is_list(value));
    return list_get_from_end((Array*) value->ptr, reverseIndex);
}
Value* list_get_safe(Value* value, int index)
{
    if (!is_list(value) || index < 0 || index >= list_length(value))
        return NULL;
    return list_get(value, index);
}

Array* list_remove_index(Array* original, int index)
{
    ice_assert(index < original->count);
    Array* result = list_touch(original);

    for (int i=index; i < result->count - 1; i++)
        swap(&result->items[i], &result->items[i+1]);
    set_null(&result->items[result->count - 1]);
    result->count--;
    return result;
}

u32 circular_shift(u32 value, int shift)
{
    shift = shift % 32;
    if (shift == 0)
        return value;
    else
        return (value << shift) | (value >> (32 - shift));
}

#endif

bool list_equals(Value left, Value right)
{
    if (!is_list(right))
        return false;

    ArrayIterator rit(right);

    for (ArrayIterator lit(left); lit.valid(); lit.advance()) {
        if (!rit.valid())
            return false;

        if (!equals(lit.current(), rit.current()))
            return false;
    }

    if (rit.valid())
        return false;

    return true;
}

Value append(Value list, Value suffix)
{
    ice_assert(is_list(list));

    if (is_empty_list(list)) {
        list = ptr_value(new_array(1, ARRAY_DEFAULT_CAPACITY_FOR_EXTEND));
        ice_assert(is_array(list));
        list.array_ptr->items[0] = suffix;
        return list;
    }

    Array* array = list.array_ptr;
    if (is_array(list) && is_writeable_object(list) && (array->length < array->capacity)) {
        // Update in place
        array->length++;
        array->items[array->length - 1] = suffix;
        return list;
    }

    ArrayNode* node = list.array_node_ptr;
    if (is_array_node(list) && is_writeable_object(list)
            && is_array(node->right) && is_writeable_object(node->right)) {

        Array* right = node->right.array_ptr;

        if (right->length < right->capacity) {
            // Update 'right' in place
            node->length++;
            right->items[right->length] = suffix;
            right->length++;
            return list;
        }
    }

    // Create a new list and cons cell
    Array* right = new_array(1, ARRAY_DEFAULT_CAPACITY_FOR_EXTEND);
    right->items[0] = suffix;
    return ptr_value(new_array_node(list, ptr_value(right)));
}

Value list_cons(Value left, Value right)
{
    ice_assert(is_list(left));
    ice_assert(is_list(right));

    if (is_empty_list(left) && is_empty_list(right))
        return empty_list();

    return ptr_value(new_array_node(left, right));
}

Value list_slice(Value list, int start_index, int len)
{
    ice_assert(start_index + len <= length(list));

    if (len == 0) {
        decref(list);
        return empty_list();
    }

    if (start_index == 0 && len == length(list))
        return list;

    if (is_array_slice(list) && object_is_writeable(list)) {
        // Modify in-place
        list.array_slice_ptr->start_index += start_index;
        list.array_slice_ptr->length = len;
        return list;
    }

    if (is_array_slice(list)) {
        // Use the slice's underlying list
        ArraySlice* asSlice = list.array_slice_ptr;
        Value base = incref(asSlice->base);
        start_index += asSlice->start_index;
        decref(list);
        return ptr_value(new_array_slice(base, start_index, len));
    }

    // Future: Could try to use underlying list for ArrayNode

    return ptr_value(new_array_slice(list, start_index, len));
}

Value set_index(Value list, int index, Value el)
{
    ice_assert(index >= 0);

    switch (list.tag) {
    case TAG_POINTER:
        switch (list.object_ptr->type) {
        case TYPE_ARRAY: {
            Array* array = list.array_ptr;
            ice_assert(index < array->length);
            if (array->refcount == 1)
                array->items[index] = el;
            incref(el);
            break;
        }
        }
    }

    return nil_value();
}

} // namespace ice
