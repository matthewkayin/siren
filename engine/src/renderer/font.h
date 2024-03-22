#pragma once

#include "defines.h"

namespace siren {
    struct Font {
        static const int FIRST_CHAR = 32;

        uint32_t atlas;
        uint32_t glyph_width;
        uint32_t glyph_height;
    };

    SIREN_API Font* font_acquire(const char* path, uint16_t size);
}