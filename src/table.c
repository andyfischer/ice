
#include "ice_internal_headers.h"

#include "list.h"
#include "table.h"
#include "value.h"

Value table0()
{
    return empty_table();
}

Value table1(Value k, Value v)
{
    Value table = list2(k, v);
    table.object->logical_type = TABLE_TYPE;
    table.object->layout = TABLE_LAYOUT_UNINDEXED_LIST;
    return table;
}

Value table2(Value k1, Value v1, Value k2, Value v2)
{
    assert(false);
    return nil_value();
}

Value table3(Value k1, Value v1, Value k2, Value v2, Value k3, Value v3)
{
    assert(false);
    return nil_value();
}

Value table4(Value k1, Value v1, Value k2, Value v2,
    Value k3, Value v3, Value k4, Value v4)
{
    assert(false);
    return nil_value();
}

Value table5(Value k1, Value v1, Value k2, Value v2, Value k3, Value v3,
    Value k4, Value v4, Value k5, Value v5)
{
    assert(false);
    return nil_value();
}

Value keys(Value table)
{
    assert(false);
    return nil_value();
}

Value values(Value table)
{
    assert(false);
    return nil_value();
}

Value insert(Value table, Value key, Value val)
{
    assert(false);
    return nil_value();
}

Value delete_key(Value table, Value key)
{
    assert(false);
    return nil_value();
}
