
#include "ice_internal_headers.h"

#include "test_framework.h"

#if 0
void lisp_test_atom()
{
    expect_str(eval(int_value(1)), "1");
}

void lisp_test_empty_list()
{
    expect_str(eval(empty_list()), "[]");
}

void lisp_test_if()
{
    expect_str(eval(list4(symbol("if"), true_value(), int_value(1), int_value(2))), "1");
    expect_str(eval(list4(symbol("if"), false_value(), int_value(1), int_value(2))), "2");
}

void lisp_test_if_parsed()
{
    expect_str(eval(parse_s("(if true 1 2)")), "1");
    expect_str(eval(parse_s("(if false 1 2)")), "2");
}

void lisp_test_equals()
{
    expect_str(eval(parse_s("(=)")), "true");
    expect_str(eval(parse_s("(= 1)")), "true");
    expect_str(eval(parse_s("(= 1 2)")), "false");
    expect_str(eval(parse_s("(= 1 1)")), "true");
    expect_str(eval(parse_s("(= 1 1 1)")), "true");
    expect_str(eval(parse_s("(= 1 1 2)")), "false");
    expect_str(eval(parse_s("(= = = =)")), "true");
    expect_str(eval(parse_s("(eq? 1 1 1)")), "true");
}

void lisp_test_let()
{
    expect_str(eval(parse_s("(let (a 1) a)")), "1");
    expect_str(eval(parse_s("(let (a 1 b 2) b)")), "2");
    expect_str(eval(parse_s("(let (a 1 b a) b)")), "1");
}

void lisp_test_square_brackets()
{
    expect_equals_and_take(eval(parse_s("[]")), "[]");
    expect_equals_and_take(eval(parse_s("[1 2 3]")), "[1, 2, 3]");
}

void test_list_eval()
{
    Value a = eval(parse_s("(list 1 2 3)"));
    expect_str(a, "[1, 2, 3]");
    decref(a);
    a = eval(parse_s("(list)"));
    expect_str(a, "[]");
    decref(a);
}

void test_append_eval()
{
    Value a = eval(parse_s("(append (list 1) 2)"));
    expect_str(a, "[1, 2]");
    decref(a);
}
#endif

void lisp_eval_test()
{
#if 0
    test_case(lisp_test_atom);
    test_case(lisp_test_empty_list);
    test_case(lisp_test_if);
    test_case(lisp_test_if_parsed);
    test_case(lisp_test_equals);
    test_case(lisp_test_let);
    test_case(lisp_test_square_brackets);
    test_case(test_list_eval);
    test_case(test_append_eval);
#endif
}
