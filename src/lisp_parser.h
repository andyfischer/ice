// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

namespace ice {

// Parse multiple whitespace-seperated s-expressions.
Value parse_multi(Value text /* consumed */);

// Parse one s-expression.
Value parse(Value text /* consumed */);

} // namespace ice
