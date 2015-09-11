// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "primitive.h"
#include "tagged_value.h"
#include "blob.h"

namespace ice {

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

Value lisp_parse_multi(Value text)
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

Value lisp_parse(Value text)
{
    Value parsed = lisp_parse_multi(text);
    if (length(parsed) == 0)
        return empty_list();

    Value first = incref(get_index(parsed, 0));
    decref(parsed);
    return first;
}

} // namespace ice
