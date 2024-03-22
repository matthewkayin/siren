#pragma once

#include "defines.h"

namespace siren {
    typedef uint32_t Texture;

    SIREN_API Texture texture_acquire(const char* path);
}