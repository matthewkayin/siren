#include "game.h"

#include <core/application.h>
#include <core/logger.h>
#include <core/input.h>
#include <renderer/font.h>
#include <renderer/renderer.h>
#include <scene/camera.h>
#include <renderer/texture.h>
#include <renderer/model.h>

#include <cstdio>

struct GameState {
    siren::FontHandle debug_font;
    siren::Camera camera;
    siren::ModelHandle test;
    siren::ModelTransform transform;
};
static GameState gamestate;

using siren::ivec2;
using siren::vec3;
using siren::quat;

bool game_init() {
    gamestate.debug_font = siren::font_acquire("font/hack.ttf", 10);
    gamestate.camera = siren::Camera();
    gamestate.test = siren::model_acquire("model/gun/gun.glb");
    if (gamestate.test == siren::MODEL_HANDLE_NULL) {
        return false;
    }
    gamestate.transform = siren::ModelTransform(gamestate.test);
    gamestate.transform.root.position = vec3(0.0f);
    gamestate.transform.root.scale = siren::vec3(0.1f);
    gamestate.transform.set_animation("fire1", false);

    return true;
}

bool game_update(float delta) {
    if (siren::application_get_mouse_mode() == siren::MOUSE_MODE_VISIBLE && siren::input_is_mouse_button_just_pressed(siren::MOUSE_BUTTON_LEFT)) { 
        siren::application_set_mouse_mode(siren::MOUSE_MODE_RELATIVE);
    } else if (siren::application_get_mouse_mode() == siren::MOUSE_MODE_RELATIVE && siren::input_is_key_just_pressed(siren::KEY_ESCAPE)) { 
        siren::application_set_mouse_mode(siren::MOUSE_MODE_VISIBLE);
    }
    bool should_handle_input = siren::application_get_mouse_mode() == siren::MOUSE_MODE_RELATIVE;

    if (should_handle_input) {
        vec3 camera_direction = vec3(0.0f);
        if (siren::input_is_key_pressed(siren::KEY_w)) {
            camera_direction += gamestate.camera.get_forward();
        }
        if (siren::input_is_key_pressed(siren::KEY_s)) {
            camera_direction -= gamestate.camera.get_forward();
        }
        if (siren::input_is_key_pressed(siren::KEY_a)) {
            camera_direction -= gamestate.camera.get_right();
        }
        if (siren::input_is_key_pressed(siren::KEY_d)) {
            camera_direction += gamestate.camera.get_right();
        }
        if (siren::input_is_key_pressed(siren::KEY_e)) {
            camera_direction += gamestate.camera.get_up();
        }
        if (siren::input_is_key_pressed(siren::KEY_q)) {
            camera_direction -= gamestate.camera.get_up();
        }

        camera_direction = camera_direction.normalized();
        vec3 camera_position = gamestate.camera.get_position();
        camera_position += camera_direction * 10.0f * delta;
        gamestate.camera.set_position(camera_position);

        ivec2 mouse_rel = siren::input_get_mouse_relative_position();
        gamestate.camera.apply_pitch((float)mouse_rel.y * -0.1f);
        gamestate.camera.apply_yaw((float)mouse_rel.x * 0.1f);
    }

    gamestate.transform.update_animation(delta);

    return true;
}

bool game_render() {
    siren::renderer_render_light(&gamestate.camera);
    siren::renderer_render_model(&gamestate.camera, gamestate.test, gamestate.transform);
    siren::renderer_render_geometry(&gamestate.camera);

    char fps_text[16];
    sprintf(fps_text, "FPS: %u", siren::application_get_fps());
    siren::renderer_render_text(fps_text, gamestate.debug_font, ivec2(0, 0), siren::vec3(1.0f));

    return true;
}