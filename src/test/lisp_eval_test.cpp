// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"
#include "test_framework.h"

#include "lisp_eval.h"
#include "lisp_parser.h"

using namespace ice;

VM* vm = NULL;

void lisp_test_atom()
{
    expect_equals(lisp_eval(vm, int_value(1)), "1");
}

void lisp_test_empty_list()
{
    expect_equals(lisp_eval(vm, list_0()), "nil");
}

void lisp_test_if()
{
    expect_equals(lisp_eval(vm, list_4(symbol("if"), true_value(), int_value(1), int_value(2))), "1");
    expect_equals(lisp_eval(vm, list_4(symbol("if"), false_value(), int_value(1), int_value(2))), "2");
}

void lisp_test_if_parsed()
{
    expect_equals(lisp_eval(vm, lisp_parse(blob_s("(if true 1 2)"))), "1");
    expect_equals(lisp_eval(vm, lisp_parse(blob_s("(if false 1 2)"))), "2");
}

void lisp_eval_test()
{
    vm = new_vm();

    test_case(lisp_test_atom);
    test_case(lisp_test_empty_list);
    test_case(lisp_test_if);
    test_case(lisp_test_if_parsed);

    free_vm(vm);
}
