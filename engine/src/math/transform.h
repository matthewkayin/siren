#pragma once

#include "defines.h"

#include "math/vector3.h"
#include "math/quaternion.h"
#include "math/matrix.h"

namespace siren {
    struct Transform {
        vec3 position;
        quat rotation;
        vec3 scale;

        bool is_dirty;
        mat4 local;
        float determinant;

        Transform* parent;

        Transform();
        Transform(vec3 position);
        Transform(quat rotation);
        Transform(vec3 position, quat rotation);
        Transform(vec3 position, quat rotation, vec3 scale);

        void set_position(vec3 position);
        void set_rotation(quat rotation);
        void set_scale(vec3 scale);
        mat4 get_matrix_local();
        mat4 get_matrix_world();
    };
}