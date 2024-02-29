#include "camera.h"

siren::Camera::Camera() {
    position = vec3(0.0f);
    direction = vec3(0.0f, 0.0f, -1.0f);
    up = vec3(0.0f, 1.0f, 0.0f);
    pitch = 0.0f;
    yaw = -90.0f;
    view_matrix = mat4::identity();
    // default to true to force an initial calculation when we first render
    dirty = true;
}

bool siren::Camera::is_dirty() const {
    return dirty;
}

siren::vec3 siren::Camera::get_position() const {
    return position;
}

void siren::Camera::set_position(vec3 value) {
    position = value;
    dirty = true;
}

siren::vec3 siren::Camera::get_direction() const {
    return direction;
}

siren::vec3 siren::Camera::get_up() const {
    return up;
}

siren::vec3 siren::Camera::get_right() const {
    return view_matrix.right();
}

void siren::Camera::look_at(vec3 point) {
    direction = position.direction_to(point);
    dirty = true;
}

void siren::Camera::apply_pitch(float value) {
    static const float PITCH_MAX = 89.0f;
    pitch = clampf(pitch + value, -89.0f, 89.0f);
    dirty = true;
}

void siren::Camera::apply_yaw(float value) {
    yaw += value;
    dirty = true;
}

siren::mat4 siren::Camera::get_view_matrix() {
    if (dirty) {
        float yaw_radians = deg_to_rad(yaw);
        float pitch_radians = deg_to_rad(pitch);

        direction = vec3(
            cos(yaw_radians) * cos(pitch_radians),
            sin(pitch_radians),
            sin(yaw_radians) * cos(pitch_radians)
        ).normalized();
        view_matrix = mat4::look_at(position, position + direction, up);

        dirty = false;
    }
    return view_matrix;
}