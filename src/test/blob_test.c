
#include "ice_internal_headers.h"

#include "test_framework.h"

#include "blob.h"
#include "block.h"
#include "value.h"

void test_is_blob()
{
    Value value = nil_value();
    expect(!is_blob(value));

    value = empty_blob();
    expect(is_blob(value));

    value = empty_list();
    expect(!is_blob(value));

    value = ptr_value(new_flat(BLOB_TYPE, 8));
    for (u8 i=0; i < 8; i++)
        value.flat->data[i] = i;
    expect(is_blob(value));
    decref(value);
}

// OLD

#if 0
void test_blob_iterator()
{
    Value value = blob_s("123");

    Value it = blob_iterator_start(value);
    expect(blob_iterator_valid(it));
    expect(blob_read_u8(it, 0) == '1');
    it = blob_advance(it, 1);
    expect(blob_iterator_valid(it));
    expect(blob_read_u8(it, 0) == '2');
    it = blob_advance(it, 1);
    expect(blob_iterator_valid(it));
    expect(blob_read_u8(it, 0) == '3');
    it = blob_advance(it, 1);
    expect(!blob_iterator_valid(it));
    decref(value);
}

void test_blob_equals_string()
{
    Value value = blob_s("apple");
    expect(is_blob(value));
    expect_str(value, "apple");

    expect(refcount(value) == 1);
    expect(blob_equals_str(value, "apple"));
    expect(refcount(value) == 1);
    expect(!blob_equals_str(value, "apple "));
    expect(refcount(value) == 1);
    expect(!blob_equals_str(value, "appl"));
    expect(!blob_equals_str(value, "banana"));
    expect(!blob_equals_str(value, ""));
    decref(value);

    value = blob_s("");
    expect(refcount(value) == 1);
    expect(blob_equals_str(value, ""));
    expect(refcount(value) == 1);
    expect(!blob_equals_str(value, " "));
    expect(refcount(value) == 1);
    decref(value);
}

void test_append_str()
{
    Value value = blob_s("");

    expect(blob_equals_str(value, ""));
    value = blob_append_s(value, "1");
    expect(blob_equals_str(value, "1"));
    value = blob_append_s_len(value, "234", 2);
    expect(blob_equals_str(value, "123"));
    value = blob_append_s_len(value, "", 0);
    expect(blob_equals_str(value, "123"));
    decref(value);
}

void test_append_byte()
{
    Value value = empty_blob();

    expect(blob_equals_str(value, ""));
    value = blob_append_u8(value, 'c');
    expect(blob_equals_str(value, "c"));
    value = blob_append_u8(value, 'a');
    expect(blob_equals_str(value, "ca"));
    value = blob_append_u8(value, 'r');
    expect(blob_equals_str(value, "car"));

    decref(value);
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

void test_blob_slice()
{
    Value source = blob_s("apple");

    expect(is_empty_blob(blob_slice(incref(source), 0, 0)));
    expect(is_empty_blob(blob_slice(incref(source), 3, 0)));
    expect(is_empty_blob(blob_slice(incref(source), 5, 0)));
    expect(is_empty_blob(blob_slice(incref(source), 6, 0)));

    Value slice1 = blob_slice(incref(source), 1, 3);
    expect_str(slice1, "ppl");

    Value slice2 = blob_slice(incref(slice1), 1, 2);
    expect_str(slice2, "pl");

    decref2(slice1, slice2);
    decref(source);
}
#endif

void blob_test()
{
    test_case(test_is_blob);

#if 0
    test_case(test_blob_equals_string);
    test_case(test_append_str);
    test_case(test_append_byte);
    test_case(test_storing_opaque_pointer);
    test_case(test_blob_slice);
#endif
}
