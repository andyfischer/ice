// Copyright (c) Andrew Fischer. See LICENSE file for license terms.

#pragma once

namespace ice {

// is_pointer
//   L0 visible only. Returns whether the value interally points to an allocated object.
bool is_object(Value value);

bool object_is_writeable(Value value);
bool is_writeable_object(Value value);
int refcount(Value value);

// Internal details
const u8 TAG_POINTER    = 0x0;
const u8 TAG_INT        = 0x1;
const u8 TAG_FLOAT      = 0x2;
const u8 TAG_EX         = 0x3;

const u8 EX_TAG_NIL            = 0x0;
const u8 EX_TAG_EMPTY_LIST      = 0x1;
const u8 EX_TAG_EMPTY_TABLE     = 0x2;
const u8 EX_TAG_EMPTY_BLOB      = 0x3;
const u8 EX_TAG_TRUE            = 0x4;
const u8 EX_TAG_FALSE           = 0x5;

// Concrete object types. Used in ObjectHeader.type
const u8 TYPE_FBLOB        = 0x1;
const u8 TYPE_FBLOB_SYMBOL = 0x2;
const u8 TYPE_HASHTABLE    = 0x3;
const u8 TYPE_ARRAY        = 0x4;
const u8 TYPE_ARRAY_NODE   = 0x5;
const u8 TYPE_ARRAY_SLICE  = 0x6;

// CValue
//
//   C++ wrapper on Value
struct CValue : Value
{
    CValue();
    ~CValue();
    CValue(Value const& rhs) { this->raw = rhs.raw; }
    CValue& operator=(CValue const&);

    int as_i();
    float as_f();
    float to_f();
    const char* as_str();

    CValue* index(int i);
    int length();

    // Assignment
    CValue* set_value(CValue* v);
    CValue* set_bool(bool b);
    CValue* set_int(int i);
    CValue* set_float(float f);

    // List utils
    CValue* set_list(int size);
    CValue* resize(int size);
    bool isEmpty();
    CValue* set_element_int(int index, int i);
    CValue* set_element_null(int index);
    CValue* set_element_str(int index, const char* s);
    CValue* set_element(int index, CValue* val);
    CValue* append();
    void pop();
    CValue* last();

    // Hashtable utils
    CValue* set_hashtable();
    CValue* int_key(int key);
    CValue* val_key(CValue* key);
    CValue* insert_int(int key);
    CValue* insert_val(CValue* valueKey);

    void operator=(Value const& rhs) { this->raw = rhs.raw; }

    // For debugging:
    void dump();
    char* to_c_string(); // Caller must free result.
};

} // namespace ice
