// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include <stdlib.h>

#include "common_headers.h"

#include "blob.h"
#include "primitive.h"
#include "tagged_value.h"

namespace ice {

i64 atoi(Value blob)
{
    blob = blob_flatten(incref(blob));

    int i = 0;
    int result = 0;
    bool sign = false;
    u8* data = &blob.fblob_ptr->data[0];
    int size = blob.fblob_ptr->size;

    if (data[0] == '-') {
        sign = true;
        i++;
    }

    for (; i < size; i++) {
        result = result *= 10;
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

f64 atof(Value blob)
{
    // TODO
    return atoi(blob);
}

Value inc(Value i)
{
    if (!is_int(i))
        return nil_value();

    return int_value(i.i + 1);
}

} // namespace ice
