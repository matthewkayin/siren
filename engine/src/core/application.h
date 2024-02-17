#pragma once

#include "defines.h"

namespace siren {
    struct ApplicationConfig {
        const char* name;
        int width;
        int height;

        bool (*init)(void* gamestate);
        bool (*update)(void* gamestate, float delta);
        bool (*render)(void* gamestate, float delta);
    };

    SIREN_API bool application_create(ApplicationConfig config);
    SIREN_API bool application_run();
}