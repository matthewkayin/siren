#pragma once

#include "defines.h"

struct PlatformState {
    void* internal_state;
};

bool platform_init(PlatformState* platform_state, const char* application_name, int x, int y, int width, int height);
void platform_quit(PlatformState* platform_state);

bool platform_pump_messages(PlatformState* platform_state);

void* platform_allocate(uint64_t size, bool aligned);
void platform_free(void* block, bool aligned);
void* platform_zero_memory(void* block, uint64_t size);
void* platform_copy_memory(void* destination, const void* source, uint64_t size);
void* platform_set_memory(void* destination, int value, uint64_t size);

void platform_console_write(const char* message, uint8_t color);
void platform_console_write_error(const char* message, uint8_t color);

double platform_get_absolute_time();

void platform_sleep(uint64_t ms);