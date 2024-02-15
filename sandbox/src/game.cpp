#include "game.h"

#include <core/siren_memory.h>

bool game_init(void* gamestate) {
    gamestate = siren::memory_allocate(sizeof(GameState), siren::MEMORY_TAG_GAME);
    return true;
}

bool game_update(void* gamestate, float delta) {
    return true;
}

bool game_render(void* gamestate, float delta) {
    return true;
}

void game_on_resize(void* gamestate, uint32_t width, uint32_t height) {
}