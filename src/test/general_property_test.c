
#include "ice_internal_headers.h"
#include "test_framework.h"

void test_hashcode_equality()
{
    Value a = next_value_any();
    Value b = next_value_any();

    if (equals(a,b))
        expect(hashcode(a) == hashcode(b));
}

void general_property_test()
{
    test_case(test_hashcode_equality);
}
