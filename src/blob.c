
#include "ice_internal_headers.h"

#include "blob.h"
#include "block.h"

Value from_str(const char* source)
{
    u32 size = strlen(source);
    Flat* flat = new_flat(BLOB_TYPE, size);
    memcpy(flat->data, source, size);
    return ptr_value(flat);
}

Value symbol(const char* source)
{
    u32 size = strlen(source);
    Flat* flat = new_flat(SYMBOL_TYPE, size);
    memcpy(flat->data, source, size);
    return ptr_value(flat);
}

void blob_print(Value blob)
{
    for_each_section(blob, it) {
        u32 len;
        u8* section = iterator_get_section(&it, &len);
        printf("%.*s", len, section);
    }
}

Value to_cstr(Value blob /*consumed*/)
{
    if (!is_blob(blob)) {
        decref(blob);
        blob = empty_blob();
    }

    blob = append_u8(blob, 0);
    blob = flatten(blob);
    return blob;
}

char* as_cstr(Value blob)
{
    assert(false);
    return NULL;
}
