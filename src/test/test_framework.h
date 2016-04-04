
#include "test_param.h"

typedef void (*test_case_func)();
void expect_(bool cond, int line, const char* file);
void expect_equals_(Value left, Value right, int line, const char* file);
void expect_str_(Value value, const char* stringRep, int line, const char* file);
void expect_equals_and_take_(Value value, const char* stringRep, int line, const char* file);
void expect_stat_within_(StatEnum stat, int count, int line, const char* file);
void expect_equiv_(bool cond1, bool cond2, int line, const char* file);
void test_case_(test_case_func func, const char* name);
void test_suite_(test_case_func func, const char* name);
void fail_current_test();

void setup_parameterized_test(int num_params);
Value test_param(int index);

#define test_case(f) test_case_((f), #f)
#define test_suite(f) void f(); test_suite_((f), #f)
#define expect_equals(a,b) expect_equals_((a),(b),__LINE__,__FILE__)
#define expect_str(a,b) expect_str_((a),(b),__LINE__,__FILE__)
#define expect_equals_and_take(a,b) expect_equals_and_take_((a),(b),__LINE__,__FILE__)
#define expect(a) expect_((a),__LINE__,__FILE__)
#define expect_stat_within(a,b) expect_stat_within_((a),(b),__LINE__,__FILE__)
#define expect_equiv(cond1,cond2) expect_equiv_((cond1),(cond2), __LINE__,__FILE__)
