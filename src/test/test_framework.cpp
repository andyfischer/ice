
#include "common_headers.h"

#include "tagged_value.h"

#include "test_framework.h"

using namespace ice;

int g_test_count;
int g_test_fail_count;

CValue g_suite_name;
CValue g_test_case_name;

struct RunningTest {
    bool failed;
    bool has_printed_header_for_test_failures;
    bool param_count;
    int* param_index;

    void nextParam() {

    }
};

RunningTest g_current_test;

void all_tests()
{
    test_suite(blob_test);
    test_suite(list_test);
    test_suite(lisp_parser_test);
    test_suite(lisp_eval_test);
    test_suite(tagged_value_test);
    test_suite(table_test);
    test_suite(primitives_test);
    test_suite(symbol_test);
}

void maybe_print_test_fail_header()
{
    if (g_current_test.has_printed_header_for_test_failures)
        return;

    print(g_suite_name);
    printf("::");
    print(g_test_case_name);
    printf(":\n");

    g_current_test.has_printed_header_for_test_failures = true;
}

void expect_(bool b, int line, const char* file)
{
    if (!b) {
        maybe_print_test_fail_header();
        printf("  expect() failed at %s:%d\n", file, line);
        fail_current_test();
    }
}

void expect_equals_(Value value, const char* stringRep, int line, const char* file)
{
    Value asstr;
    if (is_blob(value))
        asstr = incref(value);
    else
        asstr = stringify(value);

    if (!equals_str(asstr, stringRep)) {
        maybe_print_test_fail_header();
        printf("  expect_equals() failed at %s:%d\n", file, line);
        printf("    found: ");
        print(value);
        printf("\n    expected: %s\n", stringRep);
        fail_current_test();
    }

    decref(asstr);
}

void expect_equals_(ice::Value left, ice::Value right, int line, const char* file)
{
    if (!equals(left, right)) {
        maybe_print_test_fail_header();
        printf("  expect_equals() failed at %s:%d\n", file, line);
        printf("    found: ");
        print(left);
        printf("\n    expected: ");
        print(right);
        printf("\n");
        fail_current_test();
    }
}

void fail_current_test()
{
    if (g_current_test.failed)
        return;

    g_current_test.failed = true;
    g_test_fail_count++;
}

void setup_parameterized_test(int num_params)
{
}

Value test_param(int index)
{
    switch (index) {
    case 0: return nil_value();
    case 1: return int_value(0);
    case 2: return int_value(-1);
    case 3: return int_value(1);
    case 4: return int_value(2);
    case 5: return list_0();
    case 6: return list_1(int_value(0));
    }

    return nil_value();
}

int test_param_count()
{
    return 7;
}

void test_case_(test_case_func func, const char* name)
{
    g_test_count++;

    // Init g_current_test
    g_current_test.failed = false;
    g_current_test.has_printed_header_for_test_failures = false;
    g_current_test.param_count = 0;

    nullify(&g_test_case_name);
    g_test_case_name = from_str(name);

    func();
}

void test_suite_(test_case_func func, const char* name)
{
    nullify(&g_suite_name);
    g_suite_name = from_str(name);
    func();
}

#include "blob.h"

int main(int argc, char** argv)
{
    all_tests();

    printf("Ran %d test cases, %d failed.\n", g_test_count, g_test_fail_count);
}

