
#include "ice_internal_headers.h"
#include "test_framework.h"

#include "block.h"
#include "list.h"
#include "value.h"

void test_empty_list()
{
    Value value = nil_value();
    expect(!is_list(value));

    value = list0();
    expect_str(value, "[]");
    expect(is_list(value));
    expect(length(value) == 0);
    decref(value);
}

static void test_nth()
{
    Value v = empty_list();
    expect_equals(nth(v, 0), nil_value());

    v = list3(int_value(1), int_value(2), int_value(3));
    expect_equals(nth(v, 0), int_value(1));
    expect_equals(nth(v, 1), int_value(2));
    expect_equals(nth(v, 2), int_value(3));

    decref(v);
}

static void test_iterator()
{
    Value a = int_value(1);
    Value b = int_value(2);
    Value c = int_value(3);
    Value list = list3(a,b,c);

    Iterator it;
    it = iterator_start(list);
    expect(!iterator_done(&it));
    expect_equals(iterator_get_val(&it), a);
    iterator_advance_val(&it);
    expect(!iterator_done(&it));
    expect_equals(iterator_get_val(&it), b);
    iterator_advance_val(&it);
    expect(!iterator_done(&it));
    expect_equals(iterator_get_val(&it), c);
    iterator_advance_val(&it);
    expect(iterator_done(&it));
    decref(list);
}

void test_length()
{
    Value value = nil_value();
    expect(length(value) == 0);
    value = empty_list();
    expect(length(value) == 0);
    value = list1(int_value(0));
    expect(length(value) == 1);
    decref(value);
    value = list2(int_value(0), int_value(0));
    expect(length(value) == 2);
    value = append(value, int_value(3));
    expect(length(value) == 3);
    value = prepend(value, int_value(3));
    expect(length(value) == 4);
    decref(value);
}

void test_list_of_list()
{
    Value value = list3(list1(int_value(1)), list1(int_value(2)), list1(int_value(3)));
    expect_str(nth(nth(value, 0), 0), "1");
    expect_str(nth(nth(value, 1), 0), "2");
    expect_str(nth(nth(value, 2), 0), "3");

    decref(value);
}

void test_append()
{
    Value value = append(empty_list(), int_value(1));
    value = append(value, int_value(2));
    Value str = stringify(value);
    expect_str(value, "[1, 2]");
    expect_str(nth(value, 0), "1");
    expect_str(nth(value, 1), "2");
    value = append(value, int_value(3));
    expect_str(value, "[1, 2, 3]");
    expect_str(nth(value, 2), "3");
    decref2(str, value);
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

void test_list_1()
{
    Value x = list1(nil_value());
    expect_str(x, "[nil]");
    expect(length(x) == 1);
    decref(x);
}

void test_concat()
{
    Value a = list2(int_value(1), int_value(2));
    Value b = list2(int_value(3), int_value(4));

    Value x = concat(incref(a), incref(b));
    Value y = concat(b, a);
    expect_str(x, "[1, 2, 3, 4]");
    expect_str(y, "[3, 4, 1, 2]");
    expect_str(nth(x, 0), "1");
    expect_str(nth(x, 1), "2");
    expect_str(nth(x, 2), "3");
    expect_str(nth(x, 3), "4");
    decref2(x, y);
}

void test_concat_empty_left()
{
    Value x = concat(empty_list(), list2(int_value(1), int_value(2)));
    expect_str(x, "[1, 2]");
    decref(x);
}

void test_concat_empty_right()
{
    Value x = concat(list2(int_value(1), int_value(2)), empty_list());
    expect_str(x, "[1, 2]");
    decref(x);
}

void test_concat_nested()
{
    Value a = concat(list2(int_value(0), int_value(1)), list1(int_value(2)));
    Value b = concat(list1(int_value(3)), list3(int_value(4), int_value(5), int_value(6)));
    Value c = concat(a, b);
    Value d = concat(c, list0());
    Value e = concat(d, list1(int_value(7)));

    expect_str(e, "[0, 1, 2, 3, 4, 5, 6, 7]");
    for (int i = 0; i < 7; i++)
        expect_equals(nth(e, i), int_value(i));

    decref(e);
}

void test_concat_repeat()
{
    Value a = list3(int_value(1), int_value(2), int_value(3));
    Value b = concat(incref(a), incref(a));

    expect_str(b, "[1, 2, 3, 1, 2, 3]");

    b = concat(a, b);

    expect_str(b, "[1, 2, 3, 1, 2, 3, 1, 2, 3]");

    decref(b);
}

void test_slice()
{
    Value base = list4(int_value(1), int_value(2), int_value(3), int_value(4));
    Value a = slice(incref(base), 1, 2);
    expect_str(a, "[2, 3]");
    Value b = slice(incref(base), 0, 1);
    expect_str(b, "[1]");
    Value c = slice(incref(base), 0, 4);
    expect_str(c, "[1, 2, 3, 4]");
    Value d = slice(incref(base), 4, 0);
    expect_str(d, "[]");
    Value e = slice(empty_list(), 0, 0);
    expect_str(e, "[]");
    decref5(base, a, b, c, d);
}

void test_reuse_slice_of_slice()
{
    Value base = list3(int_value(1), int_value(2), int_value(3));
    Value a = slice(incref(base), 1, 2);
    Value b = slice(a, 1, 1);
    expect_str(a, "[2, 3]");
    expect_str(b, "[3]");
    decref2(base, b);
}

void test_simplify_slice_on_slice()
{
    Value base = list3(int_value(1), int_value(2), int_value(3));
    Value a = slice(incref(base), 1, 2);
    Value b = slice(incref(a), 1, 1);

    expect_str(a, "[2, 3]");
    expect_str(b, "[3]");

    expect_equals(b.slice->base, base);

    decref3(base, a, b);
}

void test_range()
{
    Value l = range(1,3);
    expect_str(l, "[1, 2]");
    decref(l);
}

void test_first()
{
    expect_str(first(empty_list()), "nil");
    expect_str(first(range(1, 2)), "1");
    expect_str(first(range(1, 4)), "1");
}

void test_rest()
{
    expect_equals_and_take(rest(list0()), "[]");
    expect_equals_and_take(rest(range(1, 2)), "[]");
    expect_equals_and_take(rest(range(1, 4)), "[2, 3]");
}

void test_set_nth()
{
    expect_equals_and_take(set_nth(range(0, 5), 2, symbol("hi")), "[0, 1, :hi, 3, 4]");
    expect_equals_and_take(set_nth(range(0, 1), 0, symbol("hi")), "[:hi]");
    expect_equals_and_take(set_nth(range(0, 3), 0, symbol("hi")), "[:hi, 1, 2]");
    expect_equals_and_take(set_nth(range(0, 3), 2, symbol("hi")), "[0, 1, :hi]");
    expect_equals_and_take(set_nth(range(0, 3), 3, symbol("hi")), "[0, 1, 2]");
}

void test_set_nth_on_empty()
{
    Value s = symbol("hi");
    expect_equals(set_nth(empty_list(), 0, symbol("hi")), empty_list());
    decref(s);
}

void list_test()
{
    test_case(test_empty_list);
    test_case(test_nth);
    test_case(test_iterator);
    test_case(test_length);
    test_case(test_list_of_list);
    test_case(test_append);
    test_case(test_append_2);
    test_case(test_list_1);
    test_case(test_concat);
    test_case(test_concat_empty_left);
    test_case(test_concat_empty_right);
    test_case(test_concat_nested);
    test_case(test_concat_repeat);
    test_case(test_slice);
    test_case(test_reuse_slice_of_slice);
    test_case(test_simplify_slice_on_slice);
    test_case(test_range);
    test_case(test_first);
    test_case(test_rest);
    test_case(test_set_nth);
    test_case(test_set_nth_on_empty);
}
