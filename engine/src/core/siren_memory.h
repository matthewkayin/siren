#pragma once

#include "defines.h"

enum MemoryTag {
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_GAME,

    MEMORY_TAG_MAX_TAGS
};

SIREN_API void memory_init();
SIREN_API void memory_quit();

SIREN_API void* siren_allocate(uint64_t size, MemoryTag tag);
SIREN_API void siren_free(void* block, uint64_t size, MemoryTag tag);
SIREN_API void* siren_zero_memory(void* block, uint64_t size);
SIREN_API void* siren_copy_memory(void* destination, const void* source, uint64_t size);
SIREN_API void* siren_set_memory(void* destination, int value, uint64_t size);
SIREN_API char* create_memory_usage_str();