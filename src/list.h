
#pragma once

#define for_each_list_item(val, it) \
    for (Iterator it = iterator_start(val); !iterator_done(&it); iterator_advance_val(&it))
