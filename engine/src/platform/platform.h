#pragma once

#include "defines.h"

struct PlatformState {
    void* internal_state;
};

SIREN_API bool platform_init(PlatformState* platform_state, const char* application_name, int x, int y, int width, int height);
SIREN_API void platform_quit(PlatformState* platform_state);

SIREN_API bool platform_pump_messages(PlatformState* platform_state);

SIREN_API void* platform_allocate(uint64_t size, bool aligned);
SIREN_API void platform_free(void* block, bool aligned);
SIREN_API void* platform_zero_memory(void* block, uint64_t size);
SIREN_API void* platform_copy_memory(void* destination, const void* source, uint64_t size);
SIREN_API void* platform_set_memory(void* destination, int value, uint64_t size);

SIREN_API void platform_console_write(const char* message, uint8_t color);
SIREN_API void platform_console_write_error(const char* message, uint8_t color);

SIREN_API double platform_get_absolute_time();

SIREN_API void platform_sleep(uint64_t ms);