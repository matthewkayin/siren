#pragma once

#include "defines.h"

#include "math/math.h"

namespace siren {
    struct Camera {
        vec3 position;
        vec3 rotation;
        mat4 view_matrix;
        bool is_dirty;

        void set_position(vec3 position);
        void set_rotation(vec3 rotation_degrees, bool clamp_x_rotation = true);
        mat4 get_view_matrix();
    };

    SIREN_API Camera camera_create();
};