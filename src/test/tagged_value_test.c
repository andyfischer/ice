
#include "ice_internal_headers.h"

#include "test_framework.h"

#include "blob.h"
#include "block.h"
#include "list.h"
#include "value.h"

void test_data_structure_sizes()
{
    expect(sizeof(Value) == 8);
}

void test_data_structure_alignment()
{
    Value value = nil_value();

    expect(value.tag == TAG_EX);
    expect(value.tag_i == TAG_EX);
    expect(value.tag_f == TAG_EX);
    expect(value.padding_i == 0);
    expect(value.padding_f == 0);
    expect(value.extag == EX_TAG_NIL);
    expect(value.tag == TAG_EX);

    value = int_value(0xffff);

    expect(value.tag == TAG_EX);
    expect(value.extag == EX_TAG_INT);
    expect(value.i == 0xffff);
    expect(value.padding_i == 0);

    value.raw = 0;
    expect(value.tag == 0);
    expect(value.i == 0);
    expect(value.padding_i == 0);

    value = nil_value();
}

void test_primitive()
{
    Value v;

    expect(!is_object(v));

    v = int_value(1);
    expect(!is_object(v));
}

void test_incref_decref()
{
    Value left;

    left = ptr_value(new_flat(BLOB_TYPE, 16));

    expect(left.object->refcount == 1);
    incref(left);
    expect(left.object->refcount == 2);
    decref(left);
    expect(left.object->refcount == 1);
    decref(left);
}

void test_hashcode_primitives()
{
    expect(hashcode(empty_list()) != hashcode(nil_value()));
    expect(hashcode(empty_list()) == hashcode(empty_list()));
    expect(hashcode(int_value(1)) != hashcode(int_value(2)));
    expect(hashcode(int_value(1)) != hashcode(float_value(1.0)));
}

void test_hashcode_lists()
{
    Value a = range(1, 3);
    Value b = slice(list4(nil_value(), int_value(1), int_value(2), nil_value()), 1, 2);
    Value c = concat(list1(int_value(1)), list1(int_value(2)));

    expect(hashcode(a) == hashcode(b));
    expect(hashcode(a) == hashcode(c));

    decref3(a,b,c);
}

void test_hashcode_symbol()
{
    Value a = symbol("a");
    Value b = symbol("b");
    Value c = symbol("c");

    expect(hashcode(a) == hashcode(a));
    expect(hashcode(a) != hashcode(b));
    expect(hashcode(a) != hashcode(c));
    expect(hashcode(b) != hashcode(c));
    decref3(a,b,c);
}

void test_deep_replace()
{
    expect_str(deep_replace(int_value(1), int_value(1), int_value(2)), "2");
    expect_str(deep_replace(int_value(1), int_value(2), int_value(3)), "1");

    Value a = list2(int_value(1), int_value(2));
    expect_str(deep_replace(a, int_value(1), int_value(3)), "[3, 2]");
    decref(a);
}

void null_pointer_is_not_an_object()
{
    Value a;
    a.object = NULL;
    expect(!is_object(a));
}

void test_opaque_pointer()
{
    int a;
    void* b = malloc(1);

    Value av = opaque_ptr(&a);
    Value bv = opaque_ptr(b);

    expect(is_opaque_pointer(av));
    expect(is_opaque_pointer(bv));

    expect(as_opaque_pointer(av) == &a);
    expect(as_opaque_pointer(bv) == b);

    decref(stringify(av));

    free(b);
}

void tagged_value_test()
{
    test_case(test_data_structure_sizes);
    test_case(test_data_structure_alignment);
    test_case(test_incref_decref);
    test_case(test_hashcode_primitives);
    test_case(test_hashcode_lists);
    test_case(test_hashcode_symbol);
    test_case(test_deep_replace);
    test_case(null_pointer_is_not_an_object);
    test_case(test_opaque_pointer);
}
