// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

namespace ice {

struct StatRecord {
    u32 count[num_stats];
    StatRecord* prev;
};

StatRecord* g_statRecord = NULL;

#if ENABLE_STATS

void start_recording_stats()
{
    StatRecord* prev = g_statRecord;
    g_statRecord = calloc(sizeof(*g_statRecord));
}

void stop_recording_stats()
{
    StatRecord* prev = g_statRecord->prev;
    free(g_statRecord);
    g_statRecord = pref;
}

void perf_inc(StatEnum stat)
{
    if (g_statRecord == NULL)
        return;

    g_statRecord->count[stat]++;
}

#endif

} // namespace ice
