#include <entry.h>

#include <core/siren_memory.h>

#include "game.h"

bool create_game(Game* game) {
    game->app_config = (ApplicationConfig) {
        .name = "Siren Sandbox",
        .x = 100,
        .y = 100,
        .width = 1280,
        .height = 720
    };
    game->init = &game_init;
    game->update = &game_update;
    game->render = &game_render;
    game->on_resize = &game_on_resize;

    game->state = siren_allocate(sizeof(GameState), MEMORY_TAG_GAME);

    return true;
}