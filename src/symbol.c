
#include "ice_internal_headers.h"

#include "block.h"
#include "symbol.h"
#include "value.h"

int g_nextGensymId = 1;

bool is_symbol(Value value)
{
    return get_logical_type(value) == SYMBOL_TYPE;
}

Value gensym(Value str)
{
    check_value(str);

    if (!is_blob(str) && !is_symbol(str))
        str = stringify(str);

    str = append_str_len(str, "#", 1);
    str = stringify_append(str, int_value(g_nextGensymId++));
    str = set_logical_type(str, SYMBOL_TYPE);
    return str;
}

