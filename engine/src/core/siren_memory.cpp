#include "siren_memory.h"

#include "core/logger.h"
#include "platform/platform.h"

#include <cstring>
#include <cstdio>

struct MemoryStats {
    uint64_t total_allocated;
    uint64_t tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "GAME       "
};
static struct MemoryStats stats;

void memory_init() {
    platform_zero_memory(&stats, sizeof(stats));
}

void memory_quit() {}

void* siren_allocate(uint64_t size, MemoryTag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        SIREN_WARN("siren_allocate called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
    }

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    // TODO: memory alignment
    void* block = platform_allocate(size, false);
    platform_zero_memory(block, size);

    return block;
}

void siren_free(void* block, uint64_t size, MemoryTag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        SIREN_WARN("siren_free called using MEMORY_TAG_UNKNOWN. Re-class this free.");
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    // TODO: memory alignment
    platform_free(block, false);
}

void* siren_zero_memory(void* block, uint64_t size) {
    return platform_zero_memory(block, size);
}

void* siren_copy_memory(void* destination, const void* source, uint64_t size) {
    return platform_copy_memory(destination, source, size);
}

void* siren_set_memory(void* destination, int value, uint64_t size) {
    return platform_set_memory(destination, value, size);
}

char* create_memory_usage_str() {
    const uint64_t gib = 1024 * 1024 * 1024;
    const uint64_t mib = 1024 * 1024; 
    const uint64_t kib = 1024; 

    char buffer[8000] = "System memory use (tagged):\n";
    uint64_t offset = strlen(buffer);
    for (uint32_t tag = 0; tag < MEMORY_TAG_MAX_TAGS; tag++) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (stats.tagged_allocations[tag] >= gib) {
            unit[0] = 'G';
            amount = stats.tagged_allocations[tag] / (float)gib;
        } else if (stats.tagged_allocations[tag] >= mib) {
            unit[0] = 'M';
            amount = stats.tagged_allocations[tag] / (float)mib;
        } else if (stats.tagged_allocations[tag] >= kib) {
            unit[0] = 'K';
            amount = stats.tagged_allocations[tag] / (float)kib;
        } else {
            unit[0] = 'B';
            unit[1] = '\0';
            amount = (float)stats.tagged_allocations[tag];
        }

        int length = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[tag], amount, unit);
        offset += length;
    }

    char* out_string = _strdup(buffer);
    return out_string;
}