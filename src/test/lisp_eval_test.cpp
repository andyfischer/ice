// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"
#include "test_framework.h"

using namespace ice;

void lisp_test_atom()
{
    expect_equals(eval(int_value(1)), "1");
}

void lisp_test_empty_list()
{
    expect_equals(eval(list_0()), "nil");
}

void lisp_test_if()
{
    expect_equals(eval(list_4(symbol("if"), true_value(), int_value(1), int_value(2))), "1");
    expect_equals(eval(list_4(symbol("if"), false_value(), int_value(1), int_value(2))), "2");
}

void lisp_test_if_parsed()
{
    expect_equals(eval(parse(blob_s("(if true 1 2)"))), "1");
    expect_equals(eval(parse(blob_s("(if false 1 2)"))), "2");
}

void lisp_eval_test()
{
    test_case(lisp_test_atom);
    test_case(lisp_test_empty_list);
    test_case(lisp_test_if);
    test_case(lisp_test_if_parsed);
}
