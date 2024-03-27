#pragma once

#include "defines.h"
#include "core/asserts.h"
#include "core/logger.h"

#define SIREN_HASHTABLE_ENTRY_NOT_FOUND UINT32_MAX

namespace siren {
    template <typename T>
    class Hashtable {
    public:
        Hashtable(uint32_t capacity = 64, float load_factor = 0.75f) {
            this->capacity = capacity;
            this->load_factor = load_factor;
            _size = 0;
            entries = (Entry*)malloc(sizeof(Entry) * capacity);
            memset(entries, 0, sizeof(Entry) * capacity);
        }

        ~Hashtable() {
            free(entries);
        }

        Hashtable(const Hashtable& other) {
            capacity = other.capacity;
            load_factor = other.load_factor;
            _size = other._size;
            entries = (Entry*)malloc(sizeof(Entry) * capacity);
            memcpy(entries, other.entries, sizeof(Entry) * capacity);
        }

        Hashtable& operator=(const Hashtable& other) {
            if (entries != nullptr) {
                free(entries);
            }

            capacity = other.capacity;
            load_factor = other.load_factor;
            _size = other._size;
            entries = (Entry*)malloc(sizeof(Entry) * capacity);
            memcpy(entries, other.entries, sizeof(Entry) * capacity);

            return *this;
        }

        Hashtable(Hashtable&& other) {
            capacity = other.capacity;
            load_factor = other.load_factor;
            _size = other._size;
            entries = other.entries;
            other.entries = nullptr;
        }

        Hashtable& operator=(Hashtable&& other) {
            if (entries != nullptr) {
                free(entries);
            }

            capacity = other.capacity;
            load_factor = other.load_factor;
            _size = other._size;
            entries = other.entries;
            other.entries = nullptr;

            return *this;
        }

        uint32_t size() const {
            return _size;
        }

        uint32_t get_index(const char* key) const {
            uint32_t hash = hash_key(key);
            if (strcmp(entries[hash].key, "") == 0) {
                return SIREN_HASHTABLE_ENTRY_NOT_FOUND;
            }

            return hash;
        }

        T& get_data(uint32_t index) {
            return entries[index].data;
        }

        const T& get_data(uint32_t index) const {
            return entries[index].data;
        }

        void reserve(uint32_t capacity) {
            uint32_t old_capacity = capacity;
            this->capacity = capacity;

            // Allocate space for new entries
            Entry* temp = entries;
            entries = (Entry*)malloc(sizeof(Entry) * capacity);
            memset(entries, 0, sizeof(Entry) * capacity);

            // Rehash old values
            for (uint32_t temp_index = 0; temp_index < old_capacity; temp_index++) {
                if (strcmp(temp[temp_index].key, "") == 0) {
                    continue;
                }

                uint32_t hash = hash_key(temp[temp_index].key);
                strcpy_s(entries[hash].key, 128, temp[temp_index].key);
                entries[hash].data = temp[temp_index].data;
            }

            // Free old entries
            free(temp);
        }

        uint32_t insert(const char* key, T value) {
            if ((float)_size / (float)capacity >= load_factor) {
                SIREN_TRACE("Hashtable with size %u, capacity %u, and load factor %f is resizing / rehashing.");
                reserve(capacity * 2);
            }

            uint32_t hash = hash_key(key);
            entries[hash].data = value;

            if (strcmp(entries[hash].key, "") == 0) {
                _size++;
            }
            strcpy_s(entries[hash].key, 128, key);

            return hash;
        }

        void remove_at(uint32_t index) {
            SIREN_ASSERT_MESSAGE(strcmp(entries[index].key, "") != 0, "Hashtable::remove_at() tried to remove entry which does not exist");
            entries[hash].key = "";
            _size++;
        }
    private:
        struct Entry {
            T data;
            char key[128];
        };

        Entry* entries;
        uint32_t capacity;
        uint32_t _size;
        float load_factor;

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