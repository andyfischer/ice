// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "ice_internal_headers.h"

#include "assert.h"

void ice_assert_(bool expr, const char* exprStr, int line, const char* file)
{
    if (!expr) {
        printf("ice_assert(%s) failed at %s:%d\n", exprStr, file, line);
        assert(false);
    }
}
