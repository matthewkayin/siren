#pragma once

#include "defines.h"

namespace siren {
    enum MemoryTag {
        MEMORY_TAG_UNKNOWN,
        MEMORY_TAG_GAME,

        MEMORY_TAG_MAX_TAGS
    };

    SIREN_API void memory_init();
    SIREN_API void memory_quit();

    SIREN_API void* memory_allocate(uint64_t size, MemoryTag tag);
    SIREN_API void memory_free(void* block, uint64_t size, MemoryTag tag);
    SIREN_API void* memory_zero(void* block, uint64_t size);
    SIREN_API void* memory_copy(void* destination, const void* source, uint64_t size);
    SIREN_API void* memory_set(void* destination, int value, uint64_t size);
    SIREN_API char* memory_create_usage_str();
}