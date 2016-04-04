
#define ICE_NO_OVERRIDE_MALLOC 1

#include "ice_internal_headers.h"

#include "managed_allocations.h"

u32 g_next_friendly_id = 1;

#define HEADER_SIGNATURE 0xab80

typedef struct AllocationHeader {
    u32 allocation_header_sig;
    u32 friendly_id;
    bool valid;
} AllocationHeader;

void internal_error(const char* msg)
{
    printf("internal error: %s\n", msg);
    assert(false);
}

void* ice_malloc(size_t size)
{
    //stat_inc(stat_alloc);
    //stat_inc(stat_malloc);

    AllocationHeader* obj = (AllocationHeader*) malloc(sizeof(AllocationHeader) + size);

    if (obj == NULL)
        internal_error("malloc failure");

    obj->allocation_header_sig = HEADER_SIGNATURE;
    obj->friendly_id = g_next_friendly_id++;
    obj->valid = true;
    return ((void*)obj) + sizeof(AllocationHeader);
}

void* ice_realloc(void* data, size_t size)
{
    if (data == NULL)
        return ice_malloc(size);

    AllocationHeader* obj = (AllocationHeader*) (data - sizeof(AllocationHeader));
    assert(obj->allocation_header_sig == HEADER_SIGNATURE);
    assert(obj->valid);

    if (size == 0) {
        ice_free(data);
        return NULL;
    }
    
    if (obj == NULL)
        internal_error("realloc failure");

    obj = realloc(obj, size);
    return ((void*) obj) + sizeof(AllocationHeader);
}

static AllocationHeader* get_header(void* data)
{
    return (AllocationHeader*) (data - sizeof(AllocationHeader));
}

void ice_free(void* data)
{
    AllocationHeader* header = get_header(data);

    assert(header->allocation_header_sig == HEADER_SIGNATURE);
    assert(header->valid);

    header->allocation_header_sig = 0;
    header->valid = false;
    free(header);
}

u32 managed_alloc_get_id(void* data)
{
    AllocationHeader* header = get_header(data);

    assert(header->allocation_header_sig == HEADER_SIGNATURE);
    assert(header->valid);

    return header->friendly_id;
}

void check_value(Value value)
{
    if (!is_object(value))
        return;

    AllocationHeader* header = get_header(value.object);
    assert(header->allocation_header_sig == HEADER_SIGNATURE);
    assert(header->valid);

    assert(value.object->refcount > 0);
}
