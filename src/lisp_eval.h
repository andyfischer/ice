// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

#include "tagged_value.h"

namespace ice {

struct VM {
    Value builtins_map;

    Value if_s;
    Value true_s;
    Value false_s;
    Value funcs;
};

VM* new_vm();
void free_vm(VM* vm);

Value lisp_eval(VM* vm, Value expr /*consumed*/);

typedef Value (*c_eval_func)(VM* vm, Value expr);

} // namespace ice
