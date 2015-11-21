// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "perf.h"

namespace ice {

struct StatRecord {
    u32 count[num_stats];
    StatRecord* prev;
};

StatRecord* g_statRecord = NULL;

void start_recording_stats()
{
    StatRecord* prev = g_statRecord;
    g_statRecord = (StatRecord*) calloc(0, sizeof(*g_statRecord));
}

void stop_recording_stats()
{
    StatRecord* prev = g_statRecord->prev;
    free(g_statRecord);
    g_statRecord = prev;
}

void stat_inc_(StatEnum stat)
{
    if (g_statRecord == NULL)
        return;

    g_statRecord->count[stat]++;
}

} // namespace ice
