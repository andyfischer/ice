
#include "ice_internal_headers.h"

#include "test_framework.h"

void test_map()
{
    Value a = range(0, 4);
    expect_str(a, "[0, 1, 2, 3]");
    a = map(a, increment);
    expect_str(a, "[1, 2, 3, 4]");
    a = map(a, increment);
    expect_str(a, "[2, 3, 4, 5]");
    decref(a);
}

Value is_even(Value i)
{
    return bool_value(i.i % 2 == 0);
}

void test_filter()
{
    Value a = range(0, 10);
    Value filtered = filter(a, is_even);
    expect_str(filtered, "[0, 2, 4, 6, 8]");
    decref(filtered);
}

void list_comprehension_test()
{
    test_case(test_map);
    test_case(test_filter);
}
