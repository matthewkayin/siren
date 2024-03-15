#include "game.h"

#include <core/application.h>
#include <math/math.h>

#include <cstdio>
#include <cstring>

int main() {
    siren::ApplicationConfig config = (siren::ApplicationConfig) {
        .name = "Siren Sandbox",
        .screen_size = siren::ivec2(1280, 720),
        .window_size = siren::ivec2(1280, 720),

        .resource_path = "../res/",

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