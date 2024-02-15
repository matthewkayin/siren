#pragma once

#include "core/siren_memory.h"
#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

extern bool create_game(siren::Game* game);

int main() {
    siren::memory_init();

    siren::Game game;
    if (!create_game(&game)) {
        SIREN_FATAL("Could not create game!");
        return -1;
    }

    if (!game.render || !game.update || !game.init || !game.on_resize) {
        SIREN_FATAL("Game function pointers must be assigned!");
        return -2;
    }

    if (!siren::application_create(&game)) {
        SIREN_INFO("Application failed to create.");
        return 1;
    }

    if (!siren::application_run()) {
        SIREN_INFO("Application did not shutdown gracefully.")
        return 2;
    }

    siren::memory_quit();

    return 0;
}