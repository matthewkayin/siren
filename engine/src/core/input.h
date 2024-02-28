#pragma once

#include "defines.h"

#include "input_keycodes.h"
#include "math/vector2.h"

namespace siren {
    enum MouseButton {
        MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_RIGHT,
        MOUSE_BUTTON_MIDDLE,
        MOUSE_BUTTON_MAX_BUTTONS
    };

    void input_init();
    void input_quit();
    void input_update();

    void input_process_key(Key key, bool pressed);

    void input_process_mouse_button(MouseButton button, bool pressed);
    void input_process_mouse_motion(ivec2 mouse_position, ivec2 mouse_relative_position);
    void input_process_mouse_wheel(int delta_z);

    SIREN_API bool input_is_key_pressed(Key key);
    SIREN_API bool input_is_key_just_pressed(Key key);
    SIREN_API bool input_is_key_just_released(Key key);

    SIREN_API bool input_is_mouse_button_pressed(MouseButton button);
    SIREN_API bool input_is_mouse_button_just_pressed(MouseButton button);
    SIREN_API bool input_is_mouse_button_just_released(MouseButton button);

    SIREN_API ivec2 input_get_mouse_position();
    SIREN_API ivec2 input_get_mouse_relative_position();
    SIREN_API int input_get_mouse_wheel_relative_position();
}
