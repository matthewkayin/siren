#pragma once

#include "defines.h"

namespace siren{
    typedef uint16_t FontId;
    const FontId FONT_ID_INVALID = 65535U;

    void font_system_init();
    void font_system_quit();
    FontId font_system_acquire_font(const char* path, uint16_t size);
}