#include "game.h"

#include <core/application.h>

#include <cstdio>

int main() {
    siren::Application app = (siren::Application) {
        .config = (siren::ApplicationConfig) {
            .name = "Siren Sandbox",
            .x = 100,
            .y = 100,
            .width = 1280,
            .height = 720
        },
        .init = &game_init,
        .update = &game_update,
        .render = &game_render,
        .on_resize = &game_on_resize,
        .gamestate = nullptr
    };
    if (!siren::application_create(&app)) {
        printf("Application failed to create!\n");
        return -1;
    }

    if (!siren::application_run()) {
        printf("Application did not quit gracefully.\n");
        return -2;
    }

    return 0;
}