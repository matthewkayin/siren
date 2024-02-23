#include "game.h"

#include <core/application.h>
#include <renderer/font.h>
#include <renderer/renderer.h>

#include <cstdio>

struct GameState {
    siren::Font* debug_font;
};
static GameState gamestate;

using siren::ivec2;
using siren::vec3;

bool game_init() {
    gamestate.debug_font = siren::font_system_acquire_font("font/hack.ttf", 10);

    return true;
}

bool game_update(float delta) {

    return true;
}

bool game_render() {
    char fps_text[16];
    sprintf(fps_text, "FPS: %u", siren::application_get_fps());
    siren::renderer_render_text(fps_text, gamestate.debug_font, ivec2(0, 0), vec3(1.0f));
    return true;
}