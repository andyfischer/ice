
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
    stat_inc(stat_alloc);
    stat_inc(stat_malloc);

    void* data = malloc(size);
    if (data == NULL)
        internal_error("malloc failure");
    return data;
}

void* ice_realloc(void* data, u32 newSize)
{
    stat_inc(stat_alloc);
    stat_inc(stat_realloc);

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
