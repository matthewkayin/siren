#include "camera.h"

siren::Camera siren::camera_create() {
    return (Camera) {
        .position = vec3(0.0f),
        .rotation = vec3(0.0f),
        .view_matrix = mat4::identity(),
        .is_dirty = true
    };
}

void siren::Camera::set_position(siren::vec3 position) {
    position = position;
    is_dirty = true;
}

void siren::Camera::set_rotation(siren::vec3 rotation_degrees, bool clamp_x_rotation) {
    rotation = vec3(deg_to_rad(rotation_degrees.x), deg_to_rad(rotation_degrees.y), deg_to_rad(rotation_degrees.z));
    if (clamp_x_rotation) {
        static const float X_LIMIT = 1.55334306f; // 89 degrees, to prevent gimbal lock
        rotation.x = clampf(rotation.x, -X_LIMIT, X_LIMIT);
    }
    is_dirty = true;
}

siren::mat4 siren::Camera::get_view_matrix() {
    if (is_dirty) {
        mat4 rotation_matrix = mat4::euler_xyz(rotation.x, rotation.y, rotation.z);
        mat4 translation_matrix = mat4::translation(position);

        view_matrix = (rotation_matrix * translation_matrix).inversed();

        is_dirty = false;
    }

    return view_matrix;
}