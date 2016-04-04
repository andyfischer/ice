
#pragma once

typedef struct Iterator {
    Value object;
    u16 offset;
    u16 end_pos;
    Value stack;
} Iterator;

Iterator iterator_start(Value obj);
bool iterator_done(Iterator* it);
void iterator_advance(Iterator* it, u16 dist);
void iterator_advance_val(Iterator* it);
void iterator_settle(Iterator* it);
u8 iterator_get_u8(Iterator* it);
Value iterator_get_val(Iterator* it);
u8* iterator_get_section(Iterator* it, u32* size);
void iterator_advance_section(Iterator* it);
void iterator_stop(Iterator* it);

#define for_each_section(val, it) \
    for (Iterator it = iterator_start(val); !iterator_done(&it); iterator_advance_section(&it))

#define for_each_byte(val, it) \
    for (Iterator it = iterator_start(val); !iterator_done(&it); iterator_advance(&it, 1))
