
#include "ice_internal_headers.h"

#include "test_framework.h"

#include "table.h"
#include "value.h"

void test_simple()
{
    Value a = empty_table();
    expect_str(a, "{}");
    a = set(a, int_value(1), int_value(2));
    expect_str(a, "{1 2}");
    a = set(a, int_value(3), int_value(4));
    expect_str(a, "{1 2, 3 4}");

    decref(a);
}

void test_simple_get()
{
    Value a = empty_table();
    a = set(a, int_value(1), int_value(2));
    a = set(a, int_value(3), int_value(4));
    
    Value b = get(a, int_value(1));
    expect_str(b, "2");
    b = get(a, int_value(2));
    expect_str(b, "nil");
    b = get(a, int_value(3));
    expect_str(b, "4");

    decref(a);
}

void test_table_keys_and_values()
{
    Value ks = keys(empty_table());
    Value vs = values(empty_table());
    expect_str(ks, "[]");
    expect_str(vs, "[]");

    Value t = empty_table();
    t = set(t, int_value(1), int_value(2));
    t = set(t, int_value(2), int_value(3));

    ks = keys(t);
    vs = values(t);
    expect_str(ks, "[1, 2]");
    expect_str(vs, "[2, 3]");
    decref3(t, ks, vs);

    // Test that order is preserved
    t = empty_table();
    t = set(t, int_value(2), int_value(4));
    t = set(t, int_value(1), int_value(3));

    ks = keys(t);
    vs = values(t);
    expect_str(ks, "[2, 1]");
    expect_str(vs, "[4, 3]");
    decref3(t, ks, vs);
}

void test_safe_writes()
{
    Value t = empty_table();
    t = set(t, list1(int_value(1)), list1(int_value(2)));
    t = set(t, list1(int_value(3)), list1(int_value(4)));

    Value t2 = incref(t);
    t2 = set(t, list1(int_value(5)), list1(int_value(6)));

    expect_str(t, "{[1] [2], [3] [4]}");
    expect_str(t2, "{[1] [2], [3] [4], [5] [6]}");

    decref2(t, t2);
}

void test_grow()
{
    Value t = empty_table();

    for (int i=0; i < 100; i++)
        t = set(t, int_value(i), int_value(100 - i));

    Value ks = keys(t);
    Value vs = values(t);

    for (int i=0; i < 100; i++) {
        expect_equals(int_value(i), nth(ks, i));
        expect_equals(int_value(100 - i), nth(vs, i));
    }

    decref3(t, ks, vs);
}

void test_grow_ownership()
{
    Value t = empty_table();

    for (int i=0; i < 10; i++)
        t = set(t, list1(int_value(i)), list1(int_value(2)));

    Value tcopy = incref(t);

    for (int i=10; i < 100; i++) {
        t = set(t, list1(int_value(i)), list1(int_value(2)));
        tcopy = set(tcopy, list1(int_value(i)), list1(int_value(2)));
    }

    expect_equals(t, tcopy);

    decref2(t, tcopy);
}

void test_as_list()
{
    Value table = empty_table();
    Value a = symbol("a");
    Value b = symbol("b");
    expect(!equals(a,b));
    table = insert(table, symbol("a"), from_str("1"));
    table = insert(table, symbol("b"), from_str("2"));
    expect(length(table) == 2);
    expect_str(nth(table, 0), "1");
    expect_str(nth(table, 1), "2");
    decref3(a,b,table);
}

void test_take_value()
{
    Value a = symbol("a");
    Value table = insert(empty_table(), incref(a), symbol("b"));
    expect_str(table, "{:a :b}");

    Value val = take_value(table, a);
    expect_str(table, "{:a nil}");
    expect_str(val, ":b");

    decref2(val, table);

    // Don't modify original table if it's being used.
    table = insert(empty_table(), incref(a), symbol("b"));
    incref(table);
    val = take_value(table, a);
    expect_str(table, "{:a :b}");
    expect_str(val, ":b");
    decref(table);
    decref3(val, table, a);
}

void test_iterator()
{
#if 0
    Value table = table3(
        symbol("a"), int_value(1),
        symbol("b"), int_value(2),
        symbol("c"), int_value(3)
    );

    Value it = table_iterator_start(table);

    expect(table_iterator_valid(it));
    expect_str(table_iterator_key(it), ":a");
    expect_str(table_iterator_value(it), "1");
    it = table_iterator_advance(it);
    expect(table_iterator_valid(it));
    expect_str(table_iterator_key(it), ":b");
    expect_str(table_iterator_value(it), "2");
    it = table_iterator_advance(it);
    expect(table_iterator_valid(it));
    expect_str(table_iterator_key(it), ":c");
    expect_str(table_iterator_value(it), "3");
    it = table_iterator_advance(it);
    expect(!table_iterator_valid(it));

    decref(table);
#endif
}

void table_test()
{
    test_case(test_simple);
#if 0
    test_case(test_simple_get);
    test_case(test_table_keys_and_values);
    test_case(test_safe_writes);
    test_case(test_grow);
    test_case(test_grow_ownership);
    test_case(test_as_list);
    test_case(test_take_value);
    test_case(test_iterator);
#endif
}

