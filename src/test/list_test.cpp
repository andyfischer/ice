
#include "common_headers.h"
#include "test_framework.h"

#include "list.h"

using namespace ice;

void test_array_struct_alignment()
{
    Array array;

    array.object_type = TYPE_ARRAY;
    array.refcount = 5;
    array.length = 6;
    array.capacity = 7;

    ArrayNode* asnode = (ArrayNode*) &array;
    expect(asnode->object_type == TYPE_ARRAY);
    expect(asnode->refcount == 5);
    expect(asnode->length == 6);
}

void test_empty_list()
{
    Value value = nil_value();
    expect(!is_list(value));

    value = list_0();
    expect_equals(value, "[]");
    expect(is_list(value));
    expect(length(value) == 0);
    decref(value);
}

void test_list_1()
{
    Value x = list_1(nil_value());
    expect_equals(x, "[nil]");
    expect(length(x) == 1);
    decref(x);
}

void test_append()
{
    Value value = append(empty_list(), int_value(1));
    value = append(value, int_value(2));
    expect_equals(value, "[1, 2]");
    expect_equals(get_index(value, 0), "1");
    expect_equals(get_index(value, 1), "2");
    value = append(value, int_value(3));
    expect_equals(value, "[1, 2, 3]");
    expect_equals(get_index(value, 2), "3");
    decref(value);
}

void test_append_2()
{
    Value list = empty_list();

    for (int i=0; i < 50; i++) {
        list = append(list, int_value(1));
        expect(length(list) == i+1);
    }

    expect(length(list) == 50);

    decref(list);
}

void test_list_eval()
{
    Value a = eval(parse_s("(list 1 2 3)"));
    expect_equals(a, "[1, 2, 3]");
    decref(a);
    a = eval(parse_s("(list)"));
    expect_equals(a, "[]");
    decref(a);
}

void test_append_eval()
{
    Value a = eval(parse_s("(append (list 1) 2)"));
    expect_equals(a, "[1, 2]");
    decref(a);
}

void test_concat()
{
    Value a = list_2(int_value(1), int_value(2));
    Value b = list_2(int_value(3), int_value(4));

    Value x = concat_2(incref(a), incref(b));
    Value y = concat_2(b, a);
    expect_equals(x, "[1, 2, 3, 4]");
    expect_equals(y, "[3, 4, 1, 2]");
    expect_equals(get_index(x, 0), "1");
    expect_equals(get_index(x, 1), "2");
    expect_equals(get_index(x, 2), "3");
    expect_equals(get_index(x, 3), "4");
    decref(x, y);
}

void test_concat_empty_left()
{
    Value x = concat_2(empty_list(), list_2(int_value(1), int_value(2)));
    expect_equals(x, "[1, 2]");
    decref(x);
}

void test_concat_empty_right()
{
    Value x = concat_2(list_2(int_value(1), int_value(2)), empty_list());
    expect_equals(x, "[1, 2]");
    decref(x);
}

void test_concat_nested()
{
    Value a = concat_2(list_2(int_value(0), int_value(1)), list_1(int_value(2)));
    Value b = concat_2(list_1(int_value(3)), list_3(int_value(4), int_value(5), int_value(6)));
    Value c = concat_2(a, b);
    Value d = concat_2(c, list_0());
    Value e = concat_2(d, list_1(int_value(7)));

    expect_equals(e, "[0, 1, 2, 3, 4, 5, 6, 7]");
    for (int i = 0; i < 7; i++)
        expect_equals(get_index(e, i), int_value(i));

    decref(e);
}

void test_concat_repeat()
{
    Value a = list_3(int_value(1), int_value(2), int_value(3));
    Value b = concat_2(incref(a), incref(a));

    expect_equals(b, "[1, 2, 3, 1, 2, 3]");

    b = concat_2(a, b);

    expect_equals(b, "[1, 2, 3, 1, 2, 3, 1, 2, 3]");

    decref(b);
}

void test_slice()
{
    Value base = list_4(int_value(1), int_value(2), int_value(3), int_value(4));
    Value a = slice(incref(base), 1, 2);
    expect_equals(a, "[2, 3]");
    Value b = slice(incref(base), 0, 1);
    expect_equals(b, "[1]");
    Value c = slice(incref(base), 0, 4);
    expect_equals(c, "[1, 2, 3, 4]");
    Value d = slice(incref(base), 4, 0);
    expect_equals(d, "[]");
    Value e = slice(empty_list(), 0, 0);
    expect_equals(e, "[]");
    decref(base, a, b, c, d);
}

void test_reuse_slice_of_slice()
{
    Value base = list_3(int_value(1), int_value(2), int_value(3));
    Value a = slice(incref(base), 1, 2);
    Value b = slice(a, 1, 1);
    expect(a == b);
    expect_equals(b, "[3]");
    decref(base, b);
}

void test_simplify_slice_on_slice()
{
    Value base = list_3(int_value(1), int_value(2), int_value(3));
    Value a = slice(incref(base), 1, 2);
    Value b = slice(incref(a), 1, 1);

    expect_equals(a, "[2, 3]");
    expect_equals(b, "[3]");

    expect(b.array_slice_ptr->base == base);

    decref(base, a, b);
}

void test_first()
{
    expect_equals_and_take(first(empty_list()), "nil");
    expect_equals_and_take(first(range(1, 2)), "1");
    expect_equals_and_take(first(range(1, 4)), "1");
}

void test_rest()
{
    expect_equals_and_take(rest(list_0()), "nil");
    expect_equals_and_take(rest(range(1, 2)), "[]");
    expect_equals_and_take(rest(range(1, 4)), "[2, 3]");
}

void test_set_index()
{
    // TODO
}

void list_test()
{
    test_case(test_array_struct_alignment);
    test_case(test_empty_list);
    test_case(test_list_1);
    test_case(test_append);
    test_case(test_append_2);
    test_case(test_list_eval);
    test_case(test_append_eval);
    test_case(test_concat);
    test_case(test_concat_empty_left);
    test_case(test_concat_empty_right);
    test_case(test_concat_nested);
    test_case(test_concat_repeat);
    test_case(test_slice);
    test_case(test_reuse_slice_of_slice);
    test_case(test_simplify_slice_on_slice);
    test_case(test_first);
    test_case(test_rest);
    test_case(test_set_index);
}
