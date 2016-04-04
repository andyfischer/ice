
#include "ice_internal_headers.h"

#include "test_framework.h"

#include "blob.h"

void symbol_test_equals()
{
    Value a = symbol("if");
    expect_str(a, ":if");
    Value b = symbol("if");
    expect_equals(a, b);
    expect(hashcode(a) == hashcode(b));
    decref2(a,b);
}

void test_gensym()
{
    Value a1 = gensym(from_str("a"));
    Value a2 = gensym(from_str("a"));
    Value a1c = incref(a1);

    expect(!equals(a1, a2));
    expect(equals(a1, a1));
    expect(equals(a2, a2));
    expect(shallow_equals(a1, a1));
    expect(shallow_equals(a1, a1c));
    expect(shallow_equals(a2, a2));

    decref3(a1, a2, a1c);
}

void symbol_test()
{
    test_case(symbol_test_equals);
    test_case(test_gensym);
}
