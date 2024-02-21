#include "game.h"

#include <core/application.h>
#include <math/math.h>

#include <cstdio>
#include <cstring>

int main(int argc, char* argv[]) {
    char resource_path[64];
    strcpy_s(resource_path, 64, "./res/");
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--resource-path") == 0 && i + 1 < argc) {
            strcpy_s(resource_path, 64, argv[i + 1]);
        }
    }

    siren::ApplicationConfig config = (siren::ApplicationConfig) {
        .name = "Siren Sandbox",
        .screen_size = siren::ivec2(1280, 720),
        .window_size = siren::ivec2(1280, 720),

        .resource_path = resource_path,

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