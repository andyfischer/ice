
#include "ice_internal_headers.h"

#include "test_framework.h"

#include "strings.h"

#if 0
void test_string_split()
{
    Value items = string_split(from_str("a b c"),
            from_str(" "));
    expect_str(items, "[\"a\", \"b\"]");
    decref(items);
}

void test_upper_lower()
{
    Value a = string_upper(from_str("Hello world!"));
    expect_str(a, "HELLO WORLD!");

    Value b = string_lower(from_str("Hello world!"));
    expect_str(b, "hello world!");
    decref2(a,b);
}

void test_trim()
{
    Value a = string_trim(from_str("  a  "));
    expect_str(a, "a");
    Value b = string_trim(from_str("  a  b"));
    expect_str(b, "a  b");
    Value c = string_trim(from_str("  "));
    expect_str(c, "");
    Value d = string_trim(from_str(""));
    expect_str(d, "");

    decref4(a,b,c,d);
}
#endif

void strings_test()
{
#if 0
    test_case(test_string_split);
    test_case(test_upper_lower);
    test_case(test_trim);
#endif
}
