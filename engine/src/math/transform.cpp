#include "transform.h"

siren::Transform siren::transform_identity() {
    return (Transform) {
        .position = vec3(0.0f, 0.0f, 0.0f),
        .rotation = quat(),
        .scale = vec3(1.0f, 1.0f, 1.0f)
    };
}

siren::mat4 siren::transform_to_matrix(const siren::Transform& transform) {
    return mat4::translate(transform.position) * transform.rotation.to_mat4() * mat4::scale(transform.scale);
}

siren::Transform siren::transform_lerp(const siren::Transform& from, const siren::Transform& to, float percent) {
    Transform result;

    result.position = from.position + ((to.position - from.position) * percent);
    result.scale = from.scale + ((to.scale - from.scale) * percent);
    result.rotation = quat::slerp(from.rotation, to.rotation, percent);

    return result;
}