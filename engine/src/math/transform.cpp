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

// basis transform

siren::BasisTransform siren::basis_transform_identity() {
    return (BasisTransform) {
        .origin = vec3(0.0f),
        .basis = mat4(1.0f)
    };
}

siren::BasisTransform siren::basis_transform_create(siren::vec3 origin, siren::vec3 rotation) {
    BasisTransform result = (BasisTransform) {
        .origin = origin,
        .basis = mat4::rotate(rotation.x, VEC3_RIGHT) * mat4::rotate(rotation.y, VEC3_UP) * mat4::rotate(rotation.z, VEC3_FORWARD)
    };

    return result;
}

siren::mat4 siren::basis_transform_to_matrix(const siren::BasisTransform& transform) {
    return mat4::translate(transform.origin) * transform.basis;
}