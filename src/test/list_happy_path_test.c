
#include "ice_internal_headers.h"

#include "test_framework.h"

#if 0
void list_append_hp()
{
    Value list = empty_list();

    for (int i=0; i < 100; i++)
        list = append(list, int_value(1));

    expect_stat_within(stat_alloc, 25);
    expect_stat_within(stat_incref, 0);

    decref(list);
}

void list_prepend_hp()
{
    Value list = empty_list();

    for (int i=0; i < 100; i++)
        list = prepend(list, int_value(1));

    expect_stat_within(stat_alloc, 25);
    expect_stat_within(stat_incref, 0);

    decref(list);
}

void list_slice_on_slice_hp()
{
    Value list = range(0, 100);

    for (int i=0; i < 100; i++)
        list = slice(list, 0, 100);

    expect_stat_within(stat_alloc, 1);

    perf_stats_reset();

    for (int i=0; i < 100; i++)
        list = slice(list, 0, i);

    expect_stat_within(stat_alloc, 2);

    decref(list);
}
#endif

void list_happy_path_test()
{
#if 0
    test_case(list_append_hp);
    test_case(list_prepend_hp);
    test_case(list_slice_on_slice_hp);
#endif
}
