
#include "common_headers.h"
#include "test_framework.h"

#include "tagged_value.h"
#include "lisp_parser.h"

using namespace ice;

void test_lisp_parse_1()
{
    Value parsed = parse(blob_s("(test 1 2 3)"));
    expect_equals(get_index(parsed, 0), ":test");
    expect(get_index(parsed, 1) == int_value(1));
    expect_equals(parsed, "[:test, 1, 2, 3]");
    decref(parsed);
}

void test_lisp_parse_nested()
{
    Value parsed = parse(blob_s("(test (4 5 6 (7 8 9)) ((10)))"));
    expect_equals(parsed, "[:test, [4, 5, 6, [7, 8, 9]], [[10]]]");
    decref(parsed);
}

void lisp_parser_test()
{
    test_case(test_lisp_parse_1);
    test_case(test_lisp_parse_nested);
}
