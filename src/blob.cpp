// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#include "common_headers.h"

#include "blob.h"
#include "tagged_value.h"

namespace ice {

// Private funcs
FBlob* fblob_resize(FBlob* fblob, u32 newSize);
FBlob* fblob_resize_rel(FBlob* fblob, i32 relsize);

FBlob* new_fblob(u32 size)
{
    FBlob* fblob = (FBlob*) ice_realloc(NULL, sizeof(*fblob) + size);
    fblob->object_type = TYPE_FBLOB;
    fblob->refcount = 1;
    fblob->size = size;
    return fblob;
}

FBlob* realloc_fblob(FBlob* fblob, u32 newSize)
{
    fblob = (FBlob*) ice_realloc(fblob, sizeof(FBlob) + newSize);
    fblob->size = newSize;
    return fblob;
}

void free_fblob(FBlob* fblob)
{
    free(fblob);
}

/* Fill fblob with a char*
     Pre: fblob must be writeable
*/
FBlob* fblob_fill(FBlob* fblob, const char* data, u32 dataSize)
{
    u32 size = std::min((u32) fblob->size, dataSize);
    memcpy(fblob->data, data, size);
    return fblob;
}

// Fill fblob with a char*
//   Pre: fblob must be writeable
FBlob* fblob_fill(FBlob* fblob, ByteIterator it)
{
    u32 size = fblob->size;

    for (; it.valid(); it.advance()) {
        if (it.offset >= size)
            break;
        fblob->data[it.offset] = it.current();
    }
    return fblob;
}

bool is_fblob(Value value)
{
    return is_object(value) && (value.object_ptr->type == TYPE_FBLOB);
}

bool is_fblob_symbol(Value value)
{
    return is_object(value) && (value.object_ptr->type == TYPE_FBLOB_SYMBOL);
}

u32 blob_size(Value blob)
{
    if (is_fblob(blob))
        return blob.fblob_ptr->size;
    return 0;
}

u32 blob_hash(Value value)
{
    // Dumb and simple hash function
    u32 result = 0;
    int byte = 0;
    for (ByteIterator it(value); it.valid(); it.advance()) {
        result = result ^ (it.current() << (8 * byte));
        byte = (byte + 1) % 4;
    }
    return result;
}

inline bool fblob_is_writable(FBlob* fblob)
{
    return fblob->refcount == 1;
}

FBlob* fblob_resize(FBlob* fblob, u32 newSize)
{
    ice_assert(newSize < 0x1000);

    if (fblob_is_writable(fblob)) {
        //printf("fblob realloc: %p %u\n", fblob, newSize);
        fblob = (FBlob*) ice_realloc(fblob, sizeof(FBlob) + newSize);
        fblob->size = newSize;
        return fblob;
    }

    FBlob* newBlob = fblob_fill(new_fblob(newSize), ByteIterator(ptr_value(fblob)));
    decref(ptr_value(fblob));
    return newBlob;
}

FBlob* fblob_resize_rel(FBlob* fblob, i32 relsize)
{
    i32 newsize = fblob->size + relsize;
    ice_assert(newsize >= 0);
    return fblob_resize(fblob, newsize);
}

bool blob_equals(Value left, Value right)
{
    ByteIterator ri(right);

    for (ByteIterator li(left); li.valid(); li.advance()) {
        if (!ri.valid())
            return false;

        if (li.current() != ri.current())
            return false;

        byte_iterator_advance(&ri);
    }

    if (ri.valid())
        return false;

    return true;
}

Value blob_resize(Value blob, u32 size)
{
    ice_assert(is_blob(blob));
    ice_assert(size < 0x1000);

    if (is_fblob(blob) && is_writeable_object(blob))
        return ptr_value(realloc_fblob(blob.fblob_ptr, size));

    FBlob* newBlob = fblob_fill(new_fblob(size), ByteIterator(blob));
    decref(blob);
    return ptr_value(newBlob);
}

Value blob_resize_rel(Value blob, i32 relsize)
{
    return blob_resize(blob, blob_size(blob) + relsize);
}

char* blob_append_writeable_section(Value* blob, u32 extra_size)
{
    ice_assert(is_blob(*blob));

    if (is_empty_blob(*blob)) {
        *blob = ptr_value(new_fblob(extra_size));
        return (char*) blob->fblob_ptr->data;
    }
        
    ice_assert(is_fblob(*blob));
    u32 existing_size = blob->fblob_ptr->size;
    *blob = blob_resize_rel(*blob, extra_size);
    return (char*) &blob->fblob_ptr->data[existing_size];
}

Value blob_append_byte(Value blob, u8 byte)
{
    char* dest = blob_append_writeable_section(&blob, 1);
    *dest = byte;
    return blob;
}

Value blob_append(Value blob, Value suffix)
{
    ice_assert(is_blob(blob));
    ice_assert(is_blob(suffix));

    ice_assert(is_fblob(blob));

    if (is_empty_blob(suffix))
        return blob;
    if (is_empty_blob(blob))
        return incref(suffix);

    u32 suffix_len = suffix.fblob_ptr->size;
    char* new_section = blob_append_writeable_section(&blob, suffix_len);
    memcpy(new_section, suffix.fblob_ptr->data, suffix_len);
    return blob;
}

Value blob_append_s(Value blob, const char* str, u32 len)
{
    ice_assert(is_blob(blob));
    char* dest = blob_append_writeable_section(&blob, len);
    memcpy(dest, str, len);
    return blob;
}

Value blob_append_s(Value blob, const char* str)
{
    return blob_append_s(blob, str, strlen(str));
}

Value blob_flatten(Value blob /*consumed*/)
{
    // Will do more later
    return blob;
}

char* blob_cstr(Value* blob /*modified*/)
{
    *blob = blob_append_byte(*blob, 0);
    *blob = blob_flatten(*blob);
    ice_assert(is_fblob(*blob));
    return (char*) blob->fblob_ptr->data;
}

#define blob_read_macro(func, type) \
    type func(Value blob, u64 byte_offset) \
    { \
        ice_assert(is_fblob(blob)); \
        FBlob* fblob = blob.fblob_ptr; \
        \
        if (byte_offset + sizeof(type) > fblob->size) \
            return 0; \
        \
        return *((type*) (&fblob->data[byte_offset])); \
    }

blob_read_macro(blob_read_u32, u32)
blob_read_macro(blob_read_u64, u64)

bool blob_equals_str(Value blob, const char* str)
{
    ByteIterator it(blob);

    for (; it.valid(); it.advance()) {
        if (str[it.offset] == 0)
            return false;

        if (str[it.offset] != it.current())
            return false;
    }
    if (str[it.offset] != 0)
        return false;

    return true;
}

void blob_print(Value blob)
{
    for (ByteIterator it(blob); it.valid(); it.advance())
        printf("%c", it.current());
}

Value blob_touch(Value blob /*consumed*/)
{
    Value out = ptr_value(fblob_fill(new_fblob(blob_size(blob)), ByteIterator(blob)));
    decref(blob);
    return out;
}

char* blob_flat_dup(Value blob)
{
    u32 len = blob_size(blob);
    char* buf = (char*) ice_malloc(len + 1);
    int i = 0;
    for (ByteIterator it(blob); it.valid(); it.advance())
        buf[i++] = it.current();
    buf[len] = 0;
    return buf;
}

void byte_iterator_start(ByteIterator* it, Value val)
{
    if (is_empty_blob(val)) {
        it->offset = 0;
        it->length = 0;
        it->data = NULL;
        return;
    }

    ice_assert(is_fblob(val) || is_fblob_symbol(val));
    FBlob* fblob = val.fblob_ptr;

    it->offset = 0;
    it->length = fblob->size;
    it->data = fblob->data;
}

void byte_iterator_start(ByteIterator* it, FBlob* fblob)
{
    it->offset = 0;
    it->length = fblob->size;
    it->data = fblob->data;
}

bool byte_iterator_valid(ByteIterator* it)
{
    return it->offset < it->length;
}

u8 byte_iterator_current(ByteIterator* it)
{
    return it->data[it->offset];
}

void byte_iterator_advance(ByteIterator* it)
{
    it->offset++;
}

ByteIterator::ByteIterator(Value val) { byte_iterator_start(this, val); }
u8 ByteIterator::current() { return byte_iterator_current(this); }
bool ByteIterator::valid() { return byte_iterator_valid(this); }
void ByteIterator::advance() { return byte_iterator_advance(this); }

} // namespace ice
