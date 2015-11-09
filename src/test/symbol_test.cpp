// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"
#include "test_framework.h"

#include "blob.h"

using namespace ice;

void symbol_test_equals()
{
    Value a = symbol("if");
    expect_equals(a, ":if");
    Value b = symbol("if");
    expect_equals(a, b);
    expect(hashcode(a) == hashcode(b));
    decref(a,b);
}

void symbol_test()
{
    test_case(symbol_test_equals);
}
