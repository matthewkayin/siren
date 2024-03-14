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
    };

    SIREN_API mat4 transform_to_matrix(const Transform& transform);
    // SIREN_API mat4 glm_transform_to_matrix(const GlmTransform& transform);
}