// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "blob.h"
#include "primitive.h"

namespace ice {

const char* chars_valid_as_symbol = "!@#$%^&*-_=+.~<>:;?/|`";

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
        if (chars_valid_as_symbol[i] == 0)
            break;
        if (chars_valid_as_symbol[i] == c)
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

Value parse_sexpr_items(ByteIterator* b)
{
    Value out = empty_list();

    while (byte_iterator_valid(b)) {

        u8 c = byte_iterator_current(b);

        if (c == ')' || c == ']')
            // stop and don't consume
            break;

        if (c == ' ') {
            byte_iterator_advance(b);
            continue;
        }

        if (c == '(') {
            byte_iterator_advance(b);
            Value sexpr = parse_sexpr_items(b);
            out = append(out, sexpr);

            if (byte_iterator_valid(b) && byte_iterator_current(b) == ')')
                byte_iterator_advance(b);

            continue;
        }

        if (c == '[') {
            byte_iterator_advance(b);
            Value sexpr = parse_sexpr_items(b);
            sexpr = prepend(sexpr, symbol("list"));
            out = append(out, sexpr);

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

    Value out = parse_sexpr_items(&b);

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

} // namespace ice
