
#include "ice_internal_headers.h"

#include "perf.h"

#define QUOTE(x) #x

u32 StatCount[num_stats];

void perf_stats_reset()
{
    memset(&StatCount, 0, sizeof(StatCount));
}

void stat_inc_(StatEnum stat)
{
    StatCount[stat]++;
}

int perf_stat_get(StatEnum e)
{
    return StatCount[e];
}

const char* perf_stat_to_string(StatEnum e)
{
#define CASE(x) case stat_##x: return QUOTE(x);

    switch (e) {
        CASE(alloc);
        CASE(malloc);
        CASE(realloc);
        CASE(incref);
        CASE(decref);
        CASE(get_index_recurse);

        case num_stats:
            return "num_stats";
    }

#undef CASE
    printf("perf_stat_to_string missing for %d\n", e);
    return "";
}
