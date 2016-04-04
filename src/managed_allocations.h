
typedef uint32_t u32;

void* ice_malloc(size_t size);
void* ice_realloc(void* data, size_t size);
void ice_free(void* data);
u32 managed_alloc_get_id(void* data);
void check_value(Value value);
