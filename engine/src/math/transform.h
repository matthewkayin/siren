#pragma once

#include "defines.h"

#include "math/vector3.h"
#include "math/matrix.h"

namespace siren {
    struct Transform {
        vec3 position;
        vec3 rotation;
        vec3 scale;
    };

    SIREN_API mat4 transform_to_matrix(const Transform& transform);
}