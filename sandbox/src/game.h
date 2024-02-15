#pragma once

#include <core/logger.h>
#include <core/application.h>
#include <core/input.h>

struct GameState {
    float delta;
};

bool game_init(void* gamestate);
bool game_update(void* gamestate, float delta);
bool game_render(void* gamestate, float delta);
void game_on_resize(void* gamestate, uint32_t width, uint32_t height);