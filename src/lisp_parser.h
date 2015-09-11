// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

namespace ice {

// Parse multiple whitespace-seperated s-expressions.
Value lisp_parse_multi(Value text /* consumed */);

// Parse one s-expression.
Value lisp_parse(Value text /* consumed */);

} // namespace ice
