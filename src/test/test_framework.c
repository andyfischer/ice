
#include <string.h>

#include "ice_internal_headers.h"

#include "value.h"
#include "perf.h"
#include "test_framework.h"

int g_test_count;
int g_test_fail_count;

const char* g_suite_name = "";
const char* g_test_case_name = "";

typedef struct RunningTest_ {
    bool failed;
    bool has_printed_header_for_test_failures;
    bool param_count;
    int* param_index;
} RunningTest;

RunningTest g_current_test;

void all_suites();

void fail_current_test()
{
    if (g_current_test.has_printed_header_for_test_failures)
        return;

    printf("Test fail: %s::%s\n", g_suite_name, g_test_case_name);

    g_current_test.has_printed_header_for_test_failures = true;

    if (!g_current_test.failed) {
        g_current_test.failed = true;
        g_test_fail_count++;
    }
}

void expect_(bool b, int line, const char* file)
{
    if (!b) {
        fail_current_test();
        printf("  expect() failed at %s:%d\n", file, line);
    }
}

void expect_str_(Value value, const char* stringRep, int line, const char* file)
{
    Value asstr;
    if (is_blob(value))
        asstr = incref(value);
    else
        asstr = stringify(value);

    if (!equals_str(asstr, stringRep)) {
        fail_current_test();
        printf("  expect_str() failed at %s:%d\n", file, line);
        printf("    found: ");
        print(value);
        printf("\n    expected: %s\n", stringRep);
    }

    decref(asstr);
}

void expect_equals_and_take_(Value value, const char* stringRep, int line, const char* file)
{
    expect_str_(value, stringRep, line, file);
    decref(value);
}

void expect_equals_(Value left, Value right, int line, const char* file)
{
    if (!equals(left, right)) {
        fail_current_test();
        printf("  expect_equals() failed at %s:%d\n", file, line);
        printf("    found: ");
        print(left);
        printf("\n    expected: ");
        print(right);
        printf("\n");
    }
}

void expect_stat_within_(StatEnum stat, int count, int line, const char* file)
{
    if (perf_stat_get(stat) > count) {
        fail_current_test();
        printf("  expect_stat_within() failed at %s:%d\n", file, line);
        printf("    stat '%s' was %d, expected <= %d\n",
            perf_stat_to_string(stat), perf_stat_get(stat), count);
    };
}

void expect_equiv_(bool cond1, bool cond2, int line, const char* file)
{
    cond1 = cond1 ? 1 : 0;
    cond2 = cond2 ? 1 : 0;
    if (cond1 != cond2) {
        fail_current_test();
        printf("  expect_equiv failed() failed at %s:%d\n", file, line);
        printf("    left condition: %d, right condition: %d\n", cond1, cond2);
    }
}

void before_each_test()
{
    // reset_test_params();
    g_current_test.failed = false;
    g_current_test.has_printed_header_for_test_failures = false;
    g_current_test.param_count = 0;
    perf_stats_reset();
}

void after_each_test()
{
    // reset_test_params();
}

void test_case_(test_case_func func, const char* name)
{
    g_test_count++;

    g_test_case_name = name;

    printf("%s::%s\n", g_suite_name, g_test_case_name);

    before_each_test();

    int loop_count = 0;
    const int max_loop_count = 10000;

    while (1) {

        func();

        break;
#if 0
        test_params_advance();
        if (test_params_done())
            break;

        loop_count++;
        if (loop_count >= max_loop_count) {
            fail_current_test();
            printf("  Hit maximum iteration count (%d) while handling params\n", max_loop_count);
            break;
        }
#endif
    }
    after_each_test();
    g_test_case_name = "";
}

void test_suite_(test_case_func func, const char* name)
{
    g_suite_name = name;
    func();
}

int main(int argc, char** argv)
{
    all_suites();

    printf("Ran %d test cases, %d failed.\n", g_test_count, g_test_fail_count);

    g_suite_name = "";
    g_test_case_name = "";
}

