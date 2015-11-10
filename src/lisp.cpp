// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "blob.h"
#include "lisp_eval.h"
#include "primitive.h"
#include "tagged_value.h"

namespace ice {

Value eval_append(Value expr)
{
    Value out = append(eval(take_index(expr, 1)), eval(take_index(expr, 2)));
    decref(expr);
    return out;
}

Value eval_concat(Value expr)
{
    Value out = concat(list_slice(expr, 1, length(expr) - 1));
    decref(expr);
    return out;
}

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

Value eval_list(Value expr)
{
    // simple because 'expr' is already a list.
    return list_slice(expr, 1, length(expr) - 1);
}

const char* symbols_valid_as_identifier = "!@#$%^&*-_=+.~<>:;?/|`";

bool is_letter(u8 c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_number(u8 c)
{
    return (c >= '0' && c <= '9');
}

bool is_space(u8 c) { return c == ' '; }

bool is_valid_inside_symbol(u8 c)
{
    if (is_letter(c) || is_number(c))
        return true;

    for (int i=0;; i++) {
        if (symbols_valid_as_identifier[i] == 0)
            break;
        if (symbols_valid_as_identifier[i] == c)
            return true;
    }

    return false;
}

Value consume_symbol(ByteIterator* b)
{
    Value match = empty_blob();

    bool integerMatch = true;
    int dotCount = 0;
    bool floatMatch = true;

    while (1) {
        u8 c = byte_iterator_current(b);

        if (!is_valid_inside_symbol(c))
            break;

        match = blob_append_byte(match, c);
        byte_iterator_advance(b);

        if (!is_number(c))
            integerMatch = false;
        if (!is_number(c) && !(c == '.'))
            floatMatch = false;
        if (c == '.')
            dotCount++;
    }

    if (integerMatch) {
        u64 result = atoi(match);
        decref(match);
        return int_value(result);
    }

    if (floatMatch && dotCount == 1) {
        f64 result = atof(match);
        decref(match);
        return float_value(result);
    }

    return to_symbol(match);
}

Value parse_list(ByteIterator* b)
{
    Value out = empty_list();

    while (byte_iterator_valid(b)) {

        u8 c = byte_iterator_current(b);

        if (c == ')')
            break;

        if (c == ' ') {
            byte_iterator_advance(b);
            continue;
        }

        if (c == '(') {
            byte_iterator_advance(b);
            Value inner_list = parse_list(b);
            out = append(out, inner_list);

            if (byte_iterator_valid(b) && byte_iterator_current(b) == ')')
                byte_iterator_advance(b);

            continue;
        }

        if (is_valid_inside_symbol(c)) {
            Value str = consume_symbol(b);
            out = append(out, str);
            continue;
        }

        // Error
        Value msg = blob_s("Unexpected character: ");
        msg = blob_append_byte(msg, c);
        out = append(out, table_1(symbol("error"), msg));
        byte_iterator_advance(b);
    }

    return out;
}

Value lisp_parse_multi(Value text /*consumed*/)
{
    if (!is_blob(text)) {
        decref(text);
        return table_1(symbol("error"), blob_s("Input to parse() must be a blob"));
    }

    ByteIterator b;
    byte_iterator_start(&b, text);

    Value out = parse_list(&b);

    decref(text);
    return out;
}

Value parse(Value text /*consumed*/)
{
    Value parsed = lisp_parse_multi(text);
    if (length(parsed) == 0)
        return empty_list();

    Value first = take_index(parsed, 0);
    decref(parsed);
    return first;
}

Value parse_s(const char* str)
{
    return parse(blob_s(str));
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

    u32 len = length(expr);

    Value function = eval(take_index(expr, 0));
    Value out;

    if (equals_symbol(function, "append"))
        out = eval_append(expr);
    else if (equals_symbol(function, "concat"))
        out = eval_concat(expr);
    else if (equals_symbol(function, "if"))
        out = eval_if(expr);
    else if (equals_symbol(function, "list"))
        out = eval_list(expr);
    else {
        decref(expr);
        return nil_value();
    }

    decref(function);
    return out;
}

} // namespace ice
