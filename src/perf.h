// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum StatEnum_ {
    stat_alloc = 0,
    stat_malloc,
    stat_realloc,
    stat_incref,
    stat_decref,
    stat_get_index_recurse,

    num_stats
} StatEnum;

void stat_inc_(StatEnum stat);

const char* perf_stat_to_string(StatEnum e);
void perf_stats_reset();
int perf_stat_get(StatEnum e);

#ifdef __cplusplus
} // extern "C"
#endif
