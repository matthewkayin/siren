#pragma once

#include "defines.h"

#include "math/math.h"

namespace siren {
    struct ApplicationConfig {
        const char* name;
        siren::ivec2 screen_size;
        siren::ivec2 window_size;

        bool (*init)(void* gamestate);
        bool (*update)(void* gamestate, float delta);
        bool (*render)(void* gamestate, float delta);
    };

    SIREN_API bool application_create(ApplicationConfig config);
    SIREN_API bool application_run();
}