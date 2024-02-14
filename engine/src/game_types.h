#pragma once

#include "core/application.h"

struct Game {
    ApplicationConfig app_config;
    bool (*init)(Game* game);
    bool (*update)(Game* game, float delta);
    bool (*render)(Game* game, float delta);
    void (*on_resize)(Game* game, uint32_t width, uint32_t height);
    void* state;
};