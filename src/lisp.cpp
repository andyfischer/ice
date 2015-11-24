// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "list.h"
#include "lisp_eval.h"
#include "primitive.h"
#include "tagged_value.h"

namespace ice {

Value args_1(Value expr /*consumed*/)
{
    Value arg = eval(take_index(expr, 1));
    decref(expr);
    return arg;
}

void args_2(Value expr /*consumed*/, Value* arg1, Value* arg2)
{
    *arg1 = eval(take_index(expr, 1));
    *arg2 = eval(take_index(expr, 2));
    decref(expr);
}

void args_3(Value expr /*consumed*/, Value* arg1, Value* arg2, Value* arg3)
{
    *arg1 = eval(take_index(expr, 1));
    *arg2 = eval(take_index(expr, 2));
    *arg3 = eval(take_index(expr, 3));
    decref(expr);
}

Value args_n(Value expr /*consumed*/)
{
    Value s = slice(expr, 1, length(expr));
    return map(s, eval);
}

Value eval_let(Value expr)
{
    Value bindings = take_index(expr, 1);

    for (int i=0; i < length(bindings); i += 2) {

        Value name = take_index(bindings, i);
        Value val = take_index(bindings, i + 1);

        bindings = deep_replace(bindings, name, val);
        expr = deep_replace(expr, name, val);

        decref(name, val);
    }

    decref(bindings);

    // Exprs
    Value out = nil_value();

    for (int i=2; i < length(expr); i++) {
        decref(out);
        out = eval(take_index(expr, i));
    }

    decref(expr);

    return out;
}

Value eval_append(Value expr)
{
    Value l, r;
    args_2(expr, &l, &r);
    return append(l, r);
}

Value eval_concat(Value expr) { return concat_n(args_n(expr)); }
Value eval_list(Value expr) { return args_n(expr); }

Value eval_print(Value expr)
{
    Value a = args_1(expr);
    print(a);
    return a;
}

Value eval_println(Value expr)
{
    Value a = args_1(expr);
    println(a);
    return a;
}

Value eval_first(Value expr) { return first(args_1(expr)); }
Value eval_rest(Value expr) { return rest(args_1(expr)); }

Value eval_if(Value expr)
{
    Value cond = eval(take_index(expr, 1));

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

Value eval_equals(Value expr)
{
    Value args = args_n(expr);
    if (length(args) < 2) {
        decref(args);
        return true_value();
    }

    Value first = take_index(args, 0);
    bool result = true;
    for (int i=1; i < length(args); i++) {
        Value x = take_index(args, i);
        result = equals(first, x);
        decref(x);

        if (!result)
            break;
    }

    decref(first, args);
    return bool_value(result);
}

Value eval_get_index(Value expr)
{
    Value list, index;
    args_2(expr, &list, &index);

    if (!is_int(index)) {
        decref(list, index);
        return list;
    }

    Value out = incref(get_index(list, index.i));
    decref(list);
    return out;
}

Value eval_set_index(Value expr)
{
    Value list, index, item;
    args_3(expr, &list, &index, &item);

    if (!is_int(index)) {
        decref(index, item);
        return list;
    }

    return set_index(list, index.i, item);
}

Value eval_deep_replace(Value expr)
{
    Value obj, target, replacement;
    args_3(expr, &obj, &target, &replacement);

    Value out = deep_replace(obj, target, replacement);
    decref(target, replacement);
    return out;
}

func_1 find_builtin_func(Value name)
{
    if (equals_symbol(name, "append"))
        return eval_append;
    else if (equals_symbol(name, "car"))
        return eval_first;
    else if (equals_symbol(name, "cdr"))
        return eval_rest;
    else if (equals_symbol(name, "concat"))
        return eval_concat;
    else if (equals_symbol(name, "if"))
        return eval_if;
    else if (equals_symbol(name, "let"))
        return eval_let;
    else if (equals_symbol(name, "list"))
        return eval_list;
    else if (equals_symbol(name, "first"))
        return eval_first;
    else if (equals_symbol(name, "rest"))
        return eval_rest;
    else if (equals_symbol(name, "print"))
        return eval_print;
    else if (equals_symbol(name, "println"))
        return eval_println;
    else if (equals_symbol(name, "="))
        return eval_equals;
    else if (equals_symbol(name, "eq?"))
        return eval_equals;
    else if (equals_symbol(name, "get-index"))
        return eval_get_index;
    else if (equals_symbol(name, "set-index"))
        return eval_set_index;
    else if (equals_symbol(name, "deep-replace"))
        return eval_deep_replace;
    
    return NULL;
}

Value eval(Value expr /*consumed*/)
{
    if (equals_symbol(expr, "true")) {
        decref(expr);
        return true_value();
    }

    if (equals_symbol(expr, "false")) {
        decref(expr);
        return false_value();
    }

    if (!is_list(expr))
        return expr;

    Value arg0 = take_index(expr, 0);
    if (is_list(arg0))
        arg0 = eval(arg0);

    if (is_symbol(arg0)) {
        func_1 builtin = find_builtin_func(arg0);
        if (builtin != NULL) {
            decref(arg0);
            return builtin(expr);
        }
    }

    expr = set_index(expr, 0, arg0);
    return expr;
}

} // namespace ice
