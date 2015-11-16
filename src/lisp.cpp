// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "list.h"
#include "lisp_eval.h"
#include "primitive.h"
#include "tagged_value.h"

namespace ice {

Value args_1(Value env, Value expr)
{
    Value arg = eval(env, take_index(expr, 1));
    decref(expr);
    return arg;
}

void args_2(Value env, Value expr, Value* arg1, Value* arg2)
{
    *arg1 = eval(env, take_index(expr, 1));
    *arg2 = eval(env, take_index(expr, 2));
    decref(expr);
}

Value args_n(Value env, Value expr)
{
    Value s = slice(expr, 1, length(expr));
    return map_1(s, eval, env);
}

Value eval_let(Value env, Value expr)
{
    env = incref(env);

    Value bindings = take_index(expr, 1);

    // Bindings
    ArrayIterator it(bindings);

    while (it.valid()) {
        Value name = incref(it.current());

        it.advance();

        Value val = nil_value();
        if (it.valid())
            val = eval(env, incref(it.current()));

        env = insert(env, name, val);
        it.advance();
    }

    decref(bindings);

    // Exprs
    Value out = nil_value();

    for (int i=2; i < length(expr); i++) {
        decref(out);
        out = eval(env, take_index(expr, i));
    }

    decref(env, expr);

    return out;
}

Value eval_append(Value env, Value expr)
{
    Value l, r;
    args_2(env, expr, &l, &r);
    return append(l, r);
}

Value eval_concat(Value env, Value expr) { return concat_n(args_n(env, expr)); }
Value eval_list(Value env, Value expr) { return args_n(env, expr); }

Value eval_print(Value env, Value expr)
{
    Value a = args_1(env, expr);
    print(a);
    return a;
}

Value eval_println(Value env, Value expr)
{
    Value a = args_1(env, expr);
    println(a);
    return a;
}

Value eval_first(Value env, Value expr) { return first(args_1(env, expr)); }
Value eval_rest(Value env, Value expr) { return rest(args_1(env, expr)); }

Value eval_if(Value env, Value expr)
{
    Value cond = eval(env, take_index(expr, 1));

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

Value eval_equals(Value env, Value expr)
{
    Value args = args_n(env, expr);
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

func_2 find_builtin_func(Value name)
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
    
    return NULL;
}

Value eval(Value env /*consumed*/, Value expr /*consumed*/)
{
    if (equals_symbol(expr, "true")) {
        decref(expr);
        return true_value();
    }

    if (equals_symbol(expr, "false")) {
        decref(expr);
        return false_value();
    }

    if (is_symbol(expr)) {
        Value val = incref(get_key(env, expr));
        if (val != nil_value()) {
            decref(expr);
            return val;
        }
    }

    if (!is_list(expr))
        return expr;

    Value arg0 = take_index(expr, 0);
    if (is_list(arg0))
        arg0 = eval(env, arg0);

    if (is_symbol(arg0)) {
        func_2 builtin = find_builtin_func(arg0);
        if (builtin != NULL) {
            decref(arg0);
            return builtin(env, expr);
        }
    }

    expr = set_index(expr, 0, arg0);
    return expr;
}

Value eval(Value expr /*consumed*/)
{
    return eval(empty_table(), expr);
}

} // namespace ice
