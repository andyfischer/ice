// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

namespace ice {

struct FBlob {
    u8 object_type; // TYPE_FBLOB
    u8 refcount;
    u16 size;
    u8 data[0];
};

// FBlob
FBlob* new_fblob(u32 size);
void free_fblob(FBlob* fblob);
FBlob* fblob_fill(FBlob* fblob, const char* data, u32 dataSize);

bool is_fblob(Value blob);
bool is_fblob_symbol(Value blob);
u32 blob_size(Value blob);
u32 blob_hash(Value value);

bool blob_equals(Value blob, Value right);
bool blob_equals_str(Value blob, const char* str);

Value blob_resize(Value blob /*consumed*/, u32 newsize);
Value blob_resize_rel(Value blob /*consumed*/, i32 relsize);
char* blob_extend(Value* value, u32 len);

Value blob_append(Value blob /*consumed*/, Value suffix /*consumed*/);
char* blob_append_writeable_section(Value* blob, u32 extra_size);
Value blob_append_byte(Value blob /*consumed*/, u8 byte);
Value blob_append_s(Value blob /*consumed*/, const char* str, u32 len);
Value blob_append_s(Value blob /*consumed*/, const char* str);
void blob_append_fblob(Value* blob, FBlob* fblob);
void blob_slice(Value* blob, int start, int end, Value* sliceOut);

void blob_print(Value blob);
Value blob_touch(Value blob /*consumed*/);

struct ByteIterator {
    u32 offset;
    u32 length;
    u8* data;

    ByteIterator() {}
    ByteIterator(Value);
    u8 current();
    bool valid();
    void advance();
};

void byte_iterator_start(ByteIterator* it, Value blob);
bool byte_iterator_valid(ByteIterator* it);
void byte_iterator_advance(ByteIterator* it);
u8 byte_iterator_current(ByteIterator* it);

} // namespace ice
