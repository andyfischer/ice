
#include "ice_internal_headers.h"

#include "test_framework.h"

#include "value.h"

#if 0
void test_lisp_parse_1()
{
    Value parsed = parse(blob_s("(test 1 2 3)"));
    expect_str(nth(parsed, 0), ":test");
    expect(nth(parsed, 1).raw == int_value(1).raw);
    expect_str(parsed, "[:test, 1, 2, 3]");
    decref(parsed);
}

void test_lisp_parse_nested()
{
    Value parsed = parse(blob_s("(test (4 5 6 (7 8 9)) ((10)))"));
    expect_str(parsed, "[:test, [4, 5, 6, [7, 8, 9]], [[10]]]");
    decref(parsed);
}
#endif

void lisp_parser_test()
{
#if 0
    test_case(test_lisp_parse_1);
    test_case(test_lisp_parse_nested);
#endif
}
