#pragma once

#include "defines.h"
#include "core/asserts.h"

namespace siren {
    template <typename T>
    class Hashtable {
    public:
        static const uint32_t ENTRY_NOT_FOUND = UINT32_MAX;

        Hashtable(uint32_t capacity) {
            this->capacity = capacity;
            entries = (Entry*)malloc(sizeof(Entry) * capacity);
            memset(entries, 0, sizeof(Entry) * capacity);
        }

        ~Hashtable() {
            free(entries);
        }

        Hashtable(const Hashtable& other) {
            capacity = other.capacity;
            entries = (Entry*)malloc(sizeof(Entry) * capacity);
            memcpy(entries, other.entries, sizeof(Entry) * capacity);
        }

        Hashtable& operator=(const Hashtable& other) {
            if (entries != nullptr) {
                free(entries);
            }

            capacity = other.capacity;
            entries = (Entry*)malloc(sizeof(Entry) * capacity);
            memcpy(entries, other.entries, sizeof(Entry) * capacity);

            return *this;
        }

        Hashtable(Hashtable&& other) {
            capacity = other.capacity;
            entries = other.entries;
            other.entries = nullptr;
        }

        Hashtable& operator=(Hashtable&& other) {
            if (entries != nullptr) {
                free(entries);
            }

            capacity = other.capacity;
            entries = other.entries;
            other.entries = nullptr;

            return *this;
        }

        uint32_t get_index_of_key(const char* key) const {
            uint32_t hash = hash_key(key);
            if (strcmp(entries[hash].key, "") == 0) {
                return ENTRY_NOT_FOUND;
            }

            return hash;
        }

        T& get_data_at_index(uint32_t index) {
            return entries[index].data;
        }

        const T& get_data_at_index(uint32_t index) const {
            return entries[index].data;
        }

        uint32_t insert(const char* key, T value) {
            uint32_t hash = hash_key(key);
            entries[hash].data = value;
            strcpy_s(entries[hash].key, 128, key);

            return hash;
        }
    private:
        struct Entry {
            T data;
            char key[128];
        };

        Entry* entries;
        uint32_t capacity;

        uint32_t hash_key(const char* key) const {
            static const uint64_t multiplier = 97;
            uint64_t hash = 0;

            for (unsigned char* us = (unsigned char*)key; *us != '\0'; us++) {
                hash = hash * multiplier + *us;
            }

            hash = hash % capacity;
            while (strcmp(entries[hash].key, key) != 0 && strcmp(entries[hash].key, "") != 0) {
                hash++;
                if (hash > capacity - 1) {
                    hash = 0;
                }
            }

            return (uint32_t)hash;
        }
    };
}