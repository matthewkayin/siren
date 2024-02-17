#include "game.h"

#include <core/application.h>

#include <cstdio>

int main() {
    siren::ApplicationConfig config = (siren::ApplicationConfig) {
        .name = "Siren Sandbox",
        .width = 1280,
        .height = 720,

        .init = &game_init,
        .update = &game_update,
        .render = &game_render
    };
    if (!siren::application_create(config)) {
        printf("Application failed to create!\n");
        return -1;
    }

    if (!siren::application_run()) {
        printf("Application did not quit gracefully.\n");
        return -2;
    }

    return 0;
}