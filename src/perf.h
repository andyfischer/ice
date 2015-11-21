// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

namespace ice {

enum StatEnum {
    stat_alloc = 0,
    stat_malloc,
    stat_realloc,

    num_stats
};

void stat_inc_(StatEnum stat);

} // namespace ice
