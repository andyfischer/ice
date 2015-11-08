// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"
#include "test_framework.h"

#include "blob.h"
#include "tagged_value.h"

using namespace ice;

void test_is_blob()
{
    Value value;
    expect(!is_blob(value));

    value = empty_blob();
    expect(is_blob(value));

    value = empty_list();
    expect(!is_blob(value));
}

void test_blob_equals_string()
{
    Value value = blob_s("apple");
    expect(is_blob(value));

    expect(blob_equals_str(value, "apple"));
    expect(!blob_equals_str(value, "apple "));
    expect(!blob_equals_str(value, "appl"));
    expect(!blob_equals_str(value, "banana"));
    expect(!blob_equals_str(value, ""));
    decref(value);

    value = blob_s("");
    expect(blob_equals_str(value, ""));
    expect(!blob_equals_str(value, " "));
    decref(value);
}

void test_append_str()
{
    Value value = blob_s("");

    expect(blob_equals_str(value, ""));
    value = blob_append_s(value, "1");
    expect(blob_equals_str(value, "1"));
    value = blob_append_s(value, "234", 2);
    expect(blob_equals_str(value, "123"));
    value = blob_append_s(value, "", 0);
    expect(blob_equals_str(value, "123"));
    decref(value);
}

void test_append_byte()
{
    Value value = empty_blob();

    expect(blob_equals_str(value, ""));
    value = blob_append_byte(value, 'c');
    expect(blob_equals_str(value, "c"));
    value = blob_append_byte(value, 'a');
    expect(blob_equals_str(value, "ca"));
    value = blob_append_byte(value, 'r');
    expect(blob_equals_str(value, "car"));

    decref(value);
}

void test_byte_iterator_empty()
{
    ByteIterator it(empty_blob());
    expect(!it.valid());
}

void test_byte_iterator_flat()
{
    Value b = blob_s("123");
    ByteIterator it(b);
    expect(it.valid());
    expect(it.current() == '1');
    it.advance();
    expect(it.valid());
    expect(it.current() == '2');
    it.advance();
    expect(it.valid());
    expect(it.current() == '3');
    it.advance();
    expect(!it.valid());
    decref(b);
}

void test_storing_opaque_pointer()
{
    if (sizeof(void*) == 8) {
        void* myptr = malloc(1);
        Value blob = blob_p(myptr);
        expect(blob_read_u64(blob, 0) == (u64) myptr);
        expect(as_pointer(blob) == myptr);
        decref(blob);
        free(myptr);
    } else if (sizeof(void*) == 4) {
        void* myptr = malloc(1);
        Value blob = blob_p(myptr);
        expect(blob_read_u32(blob, 0) == (u64) myptr);
        expect(as_pointer(blob) == myptr);
        decref(blob);
        free(myptr);
    }
}

void blob_test()
{
    test_case(test_blob_equals_string);
    test_case(test_blob_equals_string);
    test_case(test_append_str);
    test_case(test_append_byte);
    test_case(test_byte_iterator_empty);
    test_case(test_byte_iterator_flat);
    test_case(test_storing_opaque_pointer);
}
