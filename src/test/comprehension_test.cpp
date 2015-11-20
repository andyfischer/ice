
#include "common_headers.h"
#include "test_framework.h"

using namespace ice;

void test_map()
{
    Value a = range(0, 4);
    expect_equals(a, "[0, 1, 2, 3]");
    a = map(a, inc);
    expect_equals(a, "[1, 2, 3, 4]");
    a = map(a, inc);
    expect_equals(a, "[2, 3, 4, 5]");
    decref(a);
}

void list_comprehension_test()
{
    test_case(test_map);
}
