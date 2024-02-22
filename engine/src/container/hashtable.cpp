#include "hashtable.h"

#include "core/logger.h"

#include <cstdlib>
#include <cstring>

siren::Hashtable* siren::hashtable_create(uint32_t element_size, uint32_t element_count) {
    Hashtable* table = (Hashtable*)malloc(sizeof(Hashtable));

    table->element_size = element_size;
    table->element_count = element_count;
    table->data = (void*)malloc(element_size * element_count);
    memset(table->data, 0, element_size * element_count);

    return table;
}

void siren::hashtable_destroy(siren::Hashtable* table) {
    free(table->data);
    free(table);
}

uint64_t hash_key(const char* key, uint32_t element_count) {
    // multiplier is a prime number to try and avoid collisions
    static const uint64_t HASH_MULTIPLIER = 97;

    uint64_t hash = 0;

    for (unsigned const char* c = (unsigned const char*)key; *c != '\0'; c++) {
        hash = hash * HASH_MULTIPLIER + *c;
    }

    hash %= element_count;

    return hash;
}

void siren::hashtable_set(siren::Hashtable* table, const char* key, void* value) {
    uint64_t hash = hash_key(key, table->element_count);

    memcpy((char*)table->data + (hash * table->element_size), value, table->element_size);
}

void siren::hashtable_get(siren::Hashtable* table, const char* key, void* value) {
    uint64_t hash = hash_key(key, table->element_count);

    memcpy(value, (char*)table->data + (hash * table->element_size), table->element_size);
}

void siren::hashtable_set_ptr(siren::Hashtable* table, const char* key, void** value) {
    uint64_t hash = hash_key(key, table->element_count);

    ((void**)table->data)[hash] = *value;
}

void siren::hashtable_get_ptr(siren::Hashtable* table, const char* key, void** value) {
    uint64_t hash = hash_key(key, table->element_count);

    *value = ((void**)table->data)[hash];
}