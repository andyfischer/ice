
typedef void (*test_case_func)();
void expect_(bool b, int line, const char* file);
void expect_equals_(ice::Value value, const char* stringRep, int line, const char* file);
void expect_equals_(ice::Value left, ice::Value right, int line, const char* file);
void test_case_(test_case_func func, const char* name);
void test_suite_(test_case_func func, const char* name);
void fail_current_test();

void setup_parameterized_test(int num_params);
ice::Value test_param(int index);

#define test_case(f) test_case_((f), #f)
#define test_suite(f) void f(); test_suite_((f), #f)
#define expect_equals(a,b) expect_equals_((a),(b),__LINE__,__FILE__)
#define expect(a) expect_((a),__LINE__,__FILE__)
