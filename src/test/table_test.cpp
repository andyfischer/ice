// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"
#include "test_framework.h"

#include "table.h"
#include "tagged_value.h"

using namespace ice;

void test_simple()
{
    Value a = empty_table();
    expect_equals(a, "{}");
    a = set_key(a, int_value(1), int_value(2));
    expect_equals(a, "{1 -> 2}");
    a = set_key(a, int_value(3), int_value(4));
    expect_equals(a, "{1 -> 2, 3 -> 4}");

    decref(a);
}

void test_simple_get()
{
    Value a = empty_table();
    a = set_key(a, int_value(1), int_value(2));
    a = set_key(a, int_value(3), int_value(4));
    
    Value b = get_key(a, int_value(1));
    expect_equals(b, "2");
    b = get_key(a, int_value(2));
    expect_equals(b, "nil");
    b = get_key(a, int_value(3));
    expect_equals(b, "4");

    decref(a);
}

void test_table_keys_and_values()
{
    Value keys = table_keys(empty_table());
    Value values = table_values(empty_table());
    expect_equals(keys, "[]");
    expect_equals(values, "[]");

    Value t = empty_table();
    t = set_key(t, int_value(1), int_value(2));
    t = set_key(t, int_value(2), int_value(3));

    keys = table_keys(t);
    values = table_values(t);
    expect_equals(keys, "[1, 2]");
    expect_equals(values, "[2, 3]");
    decref(t, keys, values);

    // Test that order is preserved
    t = empty_table();
    t = set_key(t, int_value(2), int_value(4));
    t = set_key(t, int_value(1), int_value(3));

    keys = table_keys(t);
    values = table_values(t);
    expect_equals(keys, "[2, 1]");
    expect_equals(values, "[4, 3]");
    decref(t, keys, values);
}

void test_safe_writes()
{
    Value t = empty_table();
    t = set_key(t, list_1(int_value(1)), list_1(int_value(2)));
    t = set_key(t, list_1(int_value(3)), list_1(int_value(4)));

    Value t2 = incref(t);
    t2 = set_key(t, list_1(int_value(5)), list_1(int_value(6)));

    expect_equals(t, "{[1] -> [2], [3] -> [4]}");
    expect_equals(t2, "{[1] -> [2], [3] -> [4], [5] -> [6]}");

    decref(t, t2);
}

void test_grow()
{
    Value t = empty_table();

    for (int i=0; i < 100; i++)
        t = set_key(t, int_value(i), int_value(100 - i));

    Value keys = table_keys(t);
    Value values = table_values(t);

    for (int i=0; i < 100; i++) {
        expect_equals(int_value(i), get_index(keys, i));
        expect_equals(int_value(100 - i), get_index(values, i));
    }

    decref(t, keys, values);
}

void test_grow_ownership()
{
    Value t = empty_table();

    for (int i=0; i < 10; i++)
        t = set_key(t, list_1(int_value(i)), list_1(int_value(2)));

    Value tcopy = incref(t);

    for (int i=10; i < 100; i++) {
        t = set_key(t, list_1(int_value(i)), list_1(int_value(2)));
        tcopy = set_key(tcopy, list_1(int_value(i)), list_1(int_value(2)));
    }

    expect_equals(t, tcopy);

    decref(t, tcopy);
}

void table_test()
{
    test_case(test_simple);
    test_case(test_simple_get);
    test_case(test_table_keys_and_values);
    test_case(test_safe_writes);
    test_case(test_grow);
    test_case(test_grow_ownership);
}
