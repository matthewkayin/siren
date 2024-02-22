#pragma once

#include "defines.h"

namespace siren{
    struct Font {
        static const int FIRST_CHAR = 32;

        uint32_t atlas;
        uint32_t glyph_width;
        uint32_t glyph_height;
    };

    void font_system_init();
    void font_system_quit();
    Font* font_system_acquire_font(const char* path, uint16_t size);
}