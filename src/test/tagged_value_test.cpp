
#include "common_headers.h"
#include "test_framework.h"

#include "blob.h"
#include "list.h"
#include "tagged_value.h"

using namespace ice;

void test_data_structure_sizes()
{
    expect(sizeof(Value) == 8);
    expect(sizeof(CValue) == 8);
}

void test_data_structure_alignment()
{
    CValue value;

    expect(value.tag == TAG_EX);
    expect(value.tag_i == TAG_EX);
    expect(value.tag_f == TAG_EX);
    expect(value.padding_i == 0);
    expect(value.padding_f == 0);
    expect(value.extag == EX_TAG_NIL);
    expect(value.tag == TAG_EX);

    value = int_value(0xffff);

    expect(value.tag == TAG_INT);
    expect(value.tag == TAG_INT);
    expect(value.tag == TAG_INT);
    expect(value.tag == TAG_INT);
    expect(value.i == 0xffff);
    expect(value.padding_i == 0);

    value.raw = 0;
    expect(value.tag == 0);
    expect(value.i == 0);
    expect(value.padding_i == 0);

    init_value(&value);
}

void test_object_header_alignment()
{
    ObjectHeader header;
    FBlob* as_fblob = (FBlob*) &header;
    Array* as_array = (Array*) &header;
    ArrayNode* as_array_node = (ArrayNode*) &header;

    expect(&header.type == &as_fblob->object_type);
    expect(&header.refcount == &as_fblob->refcount);

    expect(&header.type == &as_array->object_type);
    expect(&header.refcount == &as_array->refcount);

    expect(&header.type == &as_array_node->object_type);
    expect(&header.refcount == &as_array_node->refcount);
}

void test_primitive()
{
    CValue v;

    expect(!is_object(v));

    v = int_value(1);
    expect(!is_object(v));
}

void test_incref_decref()
{
    Value left;

    left = new_blob(16);

    expect(left.object_ptr->refcount == 1);
    incref(left);
    expect(left.object_ptr->refcount == 2);
    decref(left);
    expect(left.object_ptr->refcount == 1);
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
    Value a = list_2(int_value(1), int_value(2));
    Value b = list_slice(list_4(nil_value(), int_value(1), int_value(2), nil_value()), 1, 2);
    Value c = cons(list_1(int_value(1)), list_1(int_value(2)));

    expect(hashcode(a) == hashcode(b));
    expect(hashcode(a) == hashcode(c));

    decref(a,b,c);
}

void tagged_value_test()
{
    test_case(test_data_structure_sizes);
    test_case(test_data_structure_alignment);
    test_case(test_object_header_alignment);
    test_case(test_incref_decref);
    test_case(test_hashcode_primitives);
    test_case(test_hashcode_lists);
}
