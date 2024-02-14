#pragma once

#include <game_types.h>

struct GameState {
    float delta;
};

bool game_init(Game* game);
bool game_update(Game* game, float delta);
bool game_render(Game* game, float delta);
void game_on_resize(Game* game, uint32_t width, uint32_t height);