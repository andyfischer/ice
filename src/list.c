
#include "ice_internal_headers.h"

#include "list.h"
#include "block.h"

Value list1(Value el1)
{
    Flat* flat = new_flat(LIST_TYPE, sizeof(Value));
    ((Value*) flat->data)[0] = el1;
    return ptr_value(flat);
}

Value list2(Value el1, Value el2)
{
    Flat* flat = new_flat(LIST_TYPE, sizeof(Value) * 2);
    ((Value*) flat->data)[0] = el1;
    ((Value*) flat->data)[1] = el2;
    return ptr_value(flat);
}

Value list3(Value el1, Value el2, Value el3)
{
    Flat* flat = new_flat(LIST_TYPE, sizeof(Value) * 3);
    ((Value*) flat->data)[0] = el1;
    ((Value*) flat->data)[1] = el2;
    ((Value*) flat->data)[2] = el3;
    return ptr_value(flat);
}

Value list4(Value el1, Value el2, Value el3, Value el4)
{
    Flat* flat = new_flat(LIST_TYPE, sizeof(Value) * 4);
    ((Value*) flat->data)[0] = el1;
    ((Value*) flat->data)[1] = el2;
    ((Value*) flat->data)[2] = el3;
    ((Value*) flat->data)[3] = el4;
    return ptr_value(flat);
}

Value new_list(int count)
{
    Flat* flat = new_flat(LIST_TYPE, sizeof(Value) * count);
    for (int i=0; i < count; i++)
        ((Value*) flat->data)[i] = nil_value();
    return ptr_value(flat);
}

Value take_nth(Value list /*maybe modified*/, int index)
{
    // TODO, proper reference taking
    return incref(nth(list, index));
}

Value first(Value list /* consumed */)
{
    Value result = nth(list, 0);
    decref(list);
    return result;
}

Value rest(Value list /* consumed */)
{
    return slice(list, 1, length(list) - 1);
}

Value range(int start, int fin)
{
    if (fin < start)
        fin = start;

    int count = fin - start;

    Flat* flat = new_flat(LIST_TYPE, sizeof(Value) * count);
    for (int i=0; i < count; i++)
        ((Value*) flat->data)[i] = int_value(i + start);
    return ptr_value(flat);
}

Value concat_n(Value items /*consumed*/)
{
    assert(false);
    return nil_value();
}

