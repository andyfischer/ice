
#include <stdlib.h>

#include "ice_internal_headers.h"

#include "blob.h"
#include "block.h"
#include "primitive.h"
#include "value.h"

i64 ice_atoi(Value blob)
{
    blob = flatten(incref(blob));

    int i = 0;
    int result = 0;
    bool sign = false;
    u8* data = blob.flat->data;
    int size = blob.flat->header.size;

    if (data[0] == '-') {
        sign = true;
        i++;
    }

    for (; i < size; i++) {
        result *= 10;
        char c = data[i];
        if ((c < '0') || (c > '9'))
            break;
        result += (c - '0');
    }

    if (sign)
        result *= -1;

    decref(blob);
    return result;
}

f64 ice_atof(Value blob)
{
    // TODO
    return ice_atoi(blob);
}

Value increment(Value i)
{
    if (!is_int(i))
        return nil_value();

    return int_value(i.i + 1);
}

f32 to_float(Value v)
{
    if (is_int(v))
        return v.i;
    if (is_float(v))
        return v.f;
    return 0.0;
}
