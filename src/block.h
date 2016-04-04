
#pragma once

Flat* new_flat(u8 logical_type, u16 size);
Slice* new_slice(u8 logical_type, u16 start_pos, u16 size, Value base);
Node* new_node(u8 logical_type, Value left, Value right);

bool is_flat_block(Value value);
bool is_slice_block(Value value);
bool is_node_block(Value value);

u16 block_size(Value value);

u8* block_get(Value value, u32 offset);

u8* append_writeable_section(Value* obj, u32 size);
Value append_u8(Value obj /*consumed*/, u8 val);
Value append_bytes_len(Value obj /*consumed*/, const u8* bytes, u32 size);
Value append_str(Value obj /*consumed*/, const char* str);
Value append_str_len(Value obj /*consumed*/, const char* str, u32 size);
Value concat(Value left /*consumed*/, Value right /*consumed*/);

Value byte_slice(Value base /*consumed*/, u32 start_offset, u32 size);
Value slice(Value base /*consumed*/, u32 start_offset, u32 size);
Value flatten(Value val);

