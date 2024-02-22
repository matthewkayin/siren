#pragma once

#include "defines.h"

namespace siren{
    struct Hashtable {
        uint32_t element_size;
        uint32_t element_count;
        void* data;
    };

    SIREN_API Hashtable* hashtable_create(uint32_t element_size, uint32_t element_count);
    SIREN_API void hashtable_destroy(Hashtable* table);
    SIREN_API void hashtable_set(Hashtable* table, const char* key, void* value);
    SIREN_API void hashtable_get(Hashtable* table, const char* key, void* value);
    SIREN_API void hashtable_set_ptr(Hashtable* table, const char* key, void** value);
    SIREN_API void hashtable_get_ptr(Hashtable* table, const char* key, void** value);
}