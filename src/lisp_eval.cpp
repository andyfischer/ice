// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "blob.h"
#include "lisp_eval.h"
#include "tagged_value.h"

namespace ice {

void eval_if(VM* vm, Value expr)
{
}

VM* new_vm()
{
    VM* vm = (VM*) ice_malloc(sizeof(*vm));

    vm->if_s = symbol("if");
    vm->true_s = symbol("true");
    vm->false_s = symbol("false");
    vm->funcs = empty_table();

    vm->funcs = set_key(vm->funcs, symbol("if"), blob_p((void*) eval_if));

    return vm;
}

void free_vm(VM* vm)
{
    decref(vm->if_s);
    decref(vm->true_s);
    decref(vm->false_s);
    free(vm);
}

Value lisp_eval(VM* vm, Value expr /*consumed*/)
{
    if (equals(expr, vm->true_s)) {
        decref(expr);
        return true_value();
    }

    if (equals(expr, vm->false_s)) {
        decref(expr);
        return false_value();
    }

    if (!is_list(expr))
        return expr;

    u32 len = length(expr);

    Value function = lisp_eval(vm, take_index(expr, 0));

    Value found = get_key(vm->funcs, function);

    if (!is_null(found)) {
        // TODO
    }

    // Handle 'if', using lazy evaluation.
    if (equals(function, vm->if_s)) {
        decref(function);

        Value cond = lisp_eval(vm, take_index(expr, 1));

        Value result;
        if (cond == false_value() || cond == nil_value())
            // False condition
            result = take_index(expr, 3);
        else
            // True condition
            result = take_index(expr, 2);

        decref(expr);
        return result;
    }

    return nil_value();
}

} // namespace ice
