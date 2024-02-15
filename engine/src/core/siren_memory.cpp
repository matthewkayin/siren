#include "siren_memory.h"

#include "core/logger.h"
#include "platform/platform.h"

#include <cstring>
#include <cstdio>

struct MemoryStats {
    uint64_t total_allocated;
    uint64_t tagged_allocations[siren::MEMORY_TAG_MAX_TAGS];
};

static const char* memory_tag_strings[siren::MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "GAME       "
};
static struct MemoryStats stats;

void siren::memory_init() {
    platform_memory_zero(&stats, sizeof(stats));
}

void siren::memory_quit() {}

void* siren::memory_allocate(uint64_t size, siren::MemoryTag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        SIREN_WARN("siren_allocate called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
    }

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    // TODO: memory alignment
    void* block = platform_memory_allocate(size, false);
    platform_memory_zero(block, size);

    return block;
}

void siren::memory_free(void* block, uint64_t size, siren::MemoryTag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        SIREN_WARN("siren_free called using MEMORY_TAG_UNKNOWN. Re-class this free.");
    }

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    // TODO: memory alignment
    platform_memory_free(block, false);
}

void* siren::memory_zero(void* block, uint64_t size) {
    return platform_memory_zero(block, size);
}

void* siren::memory_copy(void* destination, const void* source, uint64_t size) {
    return platform_memory_copy(destination, source, size);
}

void* siren::memory_set(void* destination, int value, uint64_t size) {
    return platform_memory_set(destination, value, size);
}

char* siren::memory_create_usage_str() {
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