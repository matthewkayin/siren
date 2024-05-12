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

        SIREN_API SIREN_INLINE static Transform identity() {
            return (Transform) {
                .position = vec3(0.0f, 0.0f, 0.0f),
                .rotation = quat(),
                .scale = vec3(1.0f, 1.0f, 1.0f)
            };
        }

        SIREN_API SIREN_INLINE static Transform lerp(const Transform& from, const Transform& to, float percent) {
            Transform result;

            result.position = from.position + ((to.position - from.position) * percent);
            result.scale = from.scale + ((to.scale - from.scale) * percent);
            result.rotation = quat::slerp(from.rotation, to.rotation, percent);

            return result;
        }

        SIREN_API SIREN_INLINE mat4 to_mat4() {
            return mat4::translate(position) * rotation.to_mat4() * mat4::scale(scale);
        }
    };
}