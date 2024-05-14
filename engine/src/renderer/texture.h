#pragma once

#include "defines.h"

#include "math/vector2.h"

#include <vector>
#include <string>

namespace siren {
    typedef uint32_t Texture;

    SIREN_API Texture texture_acquire(const char* path);
    SIREN_API Texture texture_acquire_solidcolor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    struct TextureArrayInfo {
        ivec2 size;
    };

    SIREN_API Texture texture_array_create(std::string name, const std::vector<std::string>& texture_paths);
    SIREN_API bool texture_is_texture_array(siren::Texture texture);
    SIREN_API const std::vector<TextureArrayInfo>& texture_array_info_get(siren::Texture texture);
}