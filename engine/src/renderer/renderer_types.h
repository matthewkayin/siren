#pragma once

#include "defines.h"

#include <glad/glad.h>

namespace siren {
    struct FontData {
        GLuint atlas;
        uint32_t glyph_width;
        uint32_t glyph_height;
    };
}