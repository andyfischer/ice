
#pragma once

bool is_object(Value value);

u8 get_logical_type(Value value);
u8 get_layout(Value value);
Value set_logical_type(Value value, u8 logical_type);

bool object_is_writeable(Value value);
bool is_writeable_object(Value value);
int refcount(Value value);

Value stringify_append(Value buf /*consumed*/, Value suffix);

