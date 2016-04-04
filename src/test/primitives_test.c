
#include "ice_internal_headers.h"

#include "test_framework.h"

#include "blob.h"
#include "primitive.h"
#include "value.h"

void test_atoi()
{
    Value str = from_str("123");
    expect(ice_atoi(str) == 123);
    decref(str);

    str = from_str("-51");
    expect(ice_atoi(str) == -51);
    decref(str);
}

void test_atof()
{
    return; // test disabled
    Value str = from_str("123.123");
    expect(fabs(ice_atof(str) - 123.123) < 0.0001);
    decref(str);
}

void test_int_stringify()
{
    Value str = stringify(int_value(5));
    expect_str(str, "5");
    decref(str);
}

void primitives_test()
{
    test_case(test_atoi);
    test_case(test_atof);
    test_case(test_int_stringify);
}
