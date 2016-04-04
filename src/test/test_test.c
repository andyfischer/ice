
#include "ice_internal_headers.h"

#include "test_framework.h"

void test_ndet()
{
    Value ndet = ndet_init();
    expect(ndet_next_choice(&ndet, 3) == 0);
    expect(!ndet_done(ndet));

    ndet = ndet_advance(ndet);
    expect(ndet_next_choice(&ndet, 3) == 1);
    expect(!ndet_done(ndet));

    ndet = ndet_advance(ndet);
    expect(ndet_next_choice(&ndet, 3) == 2);
    expect(!ndet_done(ndet));

    ndet = ndet_advance(ndet);
    expect(ndet_done(ndet));
}

void test_test()
{
    test_case(test_ndet);
}
