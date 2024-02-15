#pragma once

#include "defines.h"

namespace siren {
    struct PlatformState {
        void* internal_state;
    };

    bool platform_init(PlatformState* platform_state, const char* application_name, int x, int y, int width, int height);
    void platform_quit(PlatformState* platform_state);

    bool platform_pump_messages(PlatformState* platform_state);

    void* platform_memory_allocate(uint64_t size, bool aligned);
    void platform_memory_free(void* block, bool aligned);
    void* platform_memory_zero(void* block, uint64_t size);
    void* platform_memory_copy(void* destination, const void* source, uint64_t size);
    void* platform_memory_set(void* destination, int value, uint64_t size);

    void platform_console_write(const char* message, uint8_t color);
    void platform_console_write_error(const char* message, uint8_t color);

    double platform_get_absolute_time();

    void platform_sleep(uint64_t ms);
}