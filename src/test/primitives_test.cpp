// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"
#include "test_framework.h"

#include "blob.h"
#include "primitive.h"
#include "tagged_value.h"

using namespace ice;

void test_atoi()
{
    Value str = blob_s("123");
    expect(atoi(str) == 123);
    decref(str);

    str = blob_s("-51");
    expect(atoi(str) == -51);
    decref(str);
}

void test_atof()
{
    return; // test disabled
    Value str = blob_s("123.123");
    expect(fabs(atof(str) - 123.123) < 0.0001);
    decref(str);
}

void test_int_stringify()
{
    Value str = stringify(int_value(5));
    expect_equals(str, "5");
    decref(str);
}

void primitives_test()
{
    test_case(test_atoi);
    test_case(test_atof);
    test_case(test_int_stringify);
}
