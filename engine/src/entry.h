#pragma once

#include "core/siren_memory.h"
#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

extern bool create_game(Game* game);

int main() {
    memory_init();

    Game game;
    if (!create_game(&game)) {
        SIREN_FATAL("Could not create game!");
        return -1;
    }

    if (!game.render || !game.update || !game.init || !game.on_resize) {
        SIREN_FATAL("Game function pointers must be assigned!");
        return -2;
    }

    if (!application_create(&game)) {
        SIREN_INFO("Application failed to create.");
        return 1;
    }

    if (!application_run()) {
        SIREN_INFO("Application did not shutdown gracefully.")
        return 2;
    }

    memory_quit();

    return 0;
}