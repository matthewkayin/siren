#pragma once

#include "defines.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "renderer/font.h"
#include "renderer/camera.h"
#include "renderer/texture.h"

namespace siren {
    struct RendererConfig {
        const char* window_name;
        siren::ivec2 screen_size;
        siren::ivec2 window_size;
    };

    bool renderer_init(RendererConfig config);
    void renderer_quit();

    void renderer_prepare_frame();
    void renderer_present_frame();
    SIREN_API void renderer_render_text(const char* text, Font* font, ivec2 position, vec3 color);
    SIREN_API void renderer_render_cube(Camera* camera, Texture texture);
}