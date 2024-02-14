#pragma once

#include "defines.h"

struct Game;

struct ApplicationConfig {
    const char* name;
    int x;
    int y;
    int width;
    int height;
};

SIREN_API bool application_create(struct Game* game);
SIREN_API bool application_run();