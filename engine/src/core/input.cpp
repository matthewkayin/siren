#include "input.h"

#include "core/siren_memory.h"
#include "core/logger.h"

struct InputState {
    int mouse_x;
    int mouse_y;
    int8_t mouse_delta_z;
    bool key_pressed[256];
    bool mouse_button_pressed[siren::MOUSE_BUTTON_MAX_BUTTONS];
};

static bool initialized = false;
static InputState input_state_current;
static InputState input_state_previous;

void siren::input_init() {
    if (initialized) {
        return;
    }

    memory_zero(&input_state_current, sizeof(InputState));
    memory_zero(&input_state_previous, sizeof(InputState));
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

    memory_copy(&input_state_previous, &input_state_current, sizeof(InputState));
    input_state_current.mouse_delta_z = 0;
}

void siren::input_process_key(Key key, bool pressed) {
    if (!initialized) {
        return;
    }

    input_state_current.key_pressed[key] = pressed;
}

void siren::input_process_mouse_button(MouseButton button, bool pressed) {
    if (!initialized) {
        return;
    }

    input_state_current.mouse_button_pressed[button] = pressed;
}

void siren::input_process_mouse_motion(int x, int y) {
    if (!initialized) {
        return;
    }

    input_state_current.mouse_x = x;
    input_state_current.mouse_y = y;
}

void siren::input_process_mouse_wheel(int8_t delta_z) {
    if (!initialized) {
        return;
    }

    input_state_current.mouse_delta_z = delta_z;
}

bool siren::input_is_key_pressed(Key key) {
    if (!initialized) {
        return false;
    }

    return input_state_current.key_pressed[key];
}

bool siren::input_is_key_just_pressed(Key key) {
    if (!initialized) {
        return false;
    }

    return input_state_current.key_pressed[key] && !input_state_previous.key_pressed[key];
}

bool siren::input_is_key_just_released(Key key) {
    if (!initialized) {
        return false;
    }

    return !input_state_current.key_pressed[key] && input_state_previous.key_pressed[key];
}

bool siren::input_is_mouse_button_pressed(MouseButton button) {
    if (!initialized) {
        return false;
    }

    return input_state_current.mouse_button_pressed[button];
}

bool siren::input_is_mouse_button_just_pressed(MouseButton button) {
    if (!initialized) {
        return false;
    }

    return input_state_current.mouse_button_pressed[button] && !input_state_previous.mouse_button_pressed[button];
}

bool siren::input_is_mouse_button_just_released(MouseButton button) {
    if (!initialized) {
        return false;
    }

    return !input_state_current.mouse_button_pressed[button] && input_state_previous.mouse_button_pressed[button];
}

void siren::input_get_mouse_position(int* x, int* y) {
    if (!initialized) {
        return;
    }

    *x = (int)input_state_current.mouse_x;
    *y = (int)input_state_current.mouse_y;
}

void siren::input_get_mouse_relative_position(int* x, int *y) {
    if (!initialized) {
        return;
    }

    *x = (int)(input_state_current.mouse_x - input_state_previous.mouse_x);
    *y = (int)(input_state_current.mouse_y - input_state_previous.mouse_y);
}

void siren::input_get_mouse_wheel_relative_position(int* delta_z) {
    if (!initialized) {
        return;
    }

    *delta_z = (int)input_state_current.mouse_delta_z;
}