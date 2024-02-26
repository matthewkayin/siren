#pragma once

#include "defines.h"

#include "math/vector2.h"

namespace siren {
    struct ApplicationConfig {
        const char* name;
        siren::ivec2 screen_size;
        siren::ivec2 window_size;

        const char* resource_path;

        bool (*init)();
        bool (*update)(float delta);
        bool (*render)();
    };

    SIREN_API bool application_create(ApplicationConfig config);
    SIREN_API bool application_run();
    SIREN_API uint32_t application_get_fps();
}