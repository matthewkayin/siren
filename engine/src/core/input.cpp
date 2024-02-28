#include "input.h"

#include "core/logger.h"

#include <SDL2/SDL.h>

#include <cstring>

struct InputState {
    siren::ivec2 mouse_position;
    siren::ivec2 mouse_relative_position;
    int mouse_delta_z;

    bool key_pressed_current[256];
    bool key_pressed_previous[256];
    bool mouse_button_pressed_current[siren::MOUSE_BUTTON_MAX_BUTTONS];
    bool mouse_button_pressed_previous[siren::MOUSE_BUTTON_MAX_BUTTONS];
};

static bool initialized = false;
static InputState input_state;

void siren::input_init() {
    if (initialized) {
        return;
    }

    memset(&input_state, 0, sizeof(InputState));

    initialized = true;
    SIREN_INFO("Input subsystem initialized.");
}

void siren::input_quit() {
    initialized = false;
}

void siren::input_update() {
    if (!initialized) {
        return;
    }

    input_state.mouse_relative_position = ivec2();
    input_state.mouse_delta_z = 0;

    memcpy(&input_state.key_pressed_previous, &input_state.key_pressed_current, sizeof(bool) * 256);
    memcpy(&input_state.mouse_button_pressed_previous, &input_state.mouse_button_pressed_current, sizeof(bool) * siren::MOUSE_BUTTON_MAX_BUTTONS);
}

void siren::input_process_key(Key key, bool pressed) {
    if (!initialized) {
        return;
    }

    input_state.key_pressed_current[key] = pressed;
}

void siren::input_process_mouse_button(MouseButton button, bool pressed) {
    if (!initialized) {
        return;
    }

    input_state.mouse_button_pressed_current[button] = pressed;
}

void siren::input_process_mouse_motion(ivec2 mouse_position, ivec2 mouse_relative_position) {
    if (!initialized) {
        return;
    }

    input_state.mouse_position = mouse_position;
    input_state.mouse_relative_position = mouse_relative_position;
}

void siren::input_process_mouse_wheel(int delta_z) {
    if (!initialized) {
        return;
    }

    input_state.mouse_delta_z = delta_z;
}

bool siren::input_is_key_pressed(Key key) {
    if (!initialized) {
        return false;
    }

    return input_state.key_pressed_current[key];
}

bool siren::input_is_key_just_pressed(Key key) {
    if (!initialized) {
        return false;
    }

    return input_state.key_pressed_current[key] && !input_state.key_pressed_previous[key];
}

bool siren::input_is_key_just_released(Key key) {
    if (!initialized) {
        return false;
    }

    return !input_state.key_pressed_current[key] && input_state.key_pressed_previous[key];
}

bool siren::input_is_mouse_button_pressed(MouseButton button) {
    if (!initialized) {
        return false;
    }

    return input_state.mouse_button_pressed_current[button];
}

bool siren::input_is_mouse_button_just_pressed(MouseButton button) {
    if (!initialized) {
        return false;
    }

    return input_state.mouse_button_pressed_current[button] && !input_state.mouse_button_pressed_previous[button];
}

bool siren::input_is_mouse_button_just_released(MouseButton button) {
    if (!initialized) {
        return false;
    }

    return !input_state.mouse_button_pressed_current[button] && input_state.mouse_button_pressed_previous[button];
}

siren::ivec2 siren::input_get_mouse_position() {
    if (!initialized) {
        return ivec2();
    }

    return input_state.mouse_position;
}

siren::ivec2 siren::input_get_mouse_relative_position() {
    if (!initialized) {
        return ivec2();
    }

    return input_state.mouse_relative_position;
}

int siren::input_get_mouse_wheel_relative_position() {
    if (!initialized) {
        return 0;
    }

    return input_state.mouse_delta_z;
}