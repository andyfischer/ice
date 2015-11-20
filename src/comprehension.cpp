// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "list.h"
#include "comprehension.h"

namespace ice {

Value map(Value list /*consumed*/, func_1 func /* arg1 consumed */)
{
    int list_length = length(list);

    if (list_length == 0)
        return empty_list();

    Array* result = new_array(list_length, list_length);

    for (int i=0; i < list_length; i++)
        result->items[i] = func(take_index(list, i));

    decref(list);

    return ptr_value(result);
}

Value map_1(Value list /*consumed*/, func_2 func /* arg2 consumed */, Value arg1)
{
    int list_length = length(list);

    if (list_length == 0)
        return empty_list();

    Array* result = new_array(list_length, list_length);

    for (int i=0; i < list_length; i++)
        result->items[i] = func(arg1, take_index(list, i));

    decref(list);

    return ptr_value(result);
}

} // namespace ice
