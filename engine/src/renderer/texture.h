#pragma once

#include "defines.h"

namespace siren {
    typedef uint32_t Texture;

    SIREN_API Texture texture_acquire(const char* path);
    SIREN_API Texture texture_acquire_solidcolor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
}