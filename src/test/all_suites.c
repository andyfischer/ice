
#include "ice_internal_headers.h"
#include "test_framework.h"

void all_suites()
{
    test_suite(block_test);
    test_suite(blob_test);
    test_suite(list_test);
    test_suite(table_test);
#if 0
    test_suite(general_property_test);
    test_suite(list_happy_path_test);
    test_suite(list_comprehension_test);
    //test_suite(lisp_parser_test);
    //test_suite(lisp_eval_test);
    test_suite(tagged_value_test);
    test_suite(test_test);
    test_suite(primitives_test);
    test_suite(strings_test);
#endif
    test_suite(symbol_test);
}
