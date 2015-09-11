
#include "common_headers.h"

#include <cassert>

namespace ice {

void internal_error(const char* msg)
{
    printf("internal error: %s\n", msg);
    assert(false);
}

void* ice_malloc(u32 size)
{
    void* data = malloc(size);
    if (data == NULL)
        internal_error("realloc failure");
    return data;
}

void* ice_realloc(void* data, u32 newSize)
{
    void* newData = realloc(data, newSize);
    if (newData == NULL && newSize != 0)
        internal_error("realloc failure");
    return newData;
}

void ice_assert_(bool expr, const char* exprStr, int line, const char* file)
{
    if (!expr) {
        printf("ice_assert(%s) failed at %s:%d\n", exprStr, file, line);
        assert(false);
    }
}

} // namespace ice
