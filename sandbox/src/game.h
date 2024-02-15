#pragma once

#include <game_types.h>

struct GameState {
    float delta;
};

bool game_init(siren::Game* game);
bool game_update(siren::Game* game, float delta);
bool game_render(siren::Game* game, float delta);
void game_on_resize(siren::Game* game, uint32_t width, uint32_t height);