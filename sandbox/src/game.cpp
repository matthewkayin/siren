#include "game.h"

#include <cstdlib>

bool game_init(void* gamestate) {
    gamestate = malloc(sizeof(GameState));
    return true;
}

bool game_update(void* gamestate, float delta) {
    if (siren::input_is_key_just_released(siren::KEY_w)) {
        SIREN_INFO("Key released");
    }

    return true;
}

bool game_render(void* gamestate, float delta) {
    return true;
}

void game_on_resize(void* gamestate, uint32_t width, uint32_t height) {
}