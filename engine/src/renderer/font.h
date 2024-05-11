#pragma once

#include "defines.h"

namespace siren {
    struct Font {
        static const int FIRST_CHAR = 32;

        uint32_t atlas;
        uint32_t glyph_width;
        uint32_t glyph_height;
    };

    typedef uint32_t FontHandle;
    static const FontHandle FONT_HANDLE_NULL = UINT32_MAX;
    SIREN_API FontHandle font_acquire(const char* path, uint16_t size);
    const Font& font_get(FontHandle handle);
}