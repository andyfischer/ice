// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

namespace ice {

#if ENABLE_STATS

enum StatEnum {
    stat_alloc = 0,
    stat_malloc,
    stat_realloc,

    num_stats
};

void perf_inc(StatEnum stat);

#else

#define perf_inc(x) ;

#endif

} // namespace ice
