#pragma once

#include "defines.h"

namespace siren {
    struct ApplicationConfig {
        const char* name;
        int x;
        int y;
        int width;
        int height;
    };

    struct Application {
        ApplicationConfig config;
        bool (*init)(void* gamestate);
        bool (*update)(void* gamestate, float delta);
        bool (*render)(void* gamestate, float delta);
        void (*on_resize)(void* gamestate, uint32_t width, uint32_t height);
        void* gamestate;
    };

    SIREN_API bool application_create(Application* app);
    SIREN_API bool application_run();
}