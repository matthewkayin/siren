#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

siren::Camera::Camera() {
    position = vec3(0.0f);
    forward = vec3(0.0f, 0.0f, -1.0f);
    up = vec3(0.0f, 1.0f, 0.0f);
    pitch = 0.0f;
    yaw = -90.0f;
    view_matrix = mat4(1.0f);
    // default to true to force an initial calculation when we first render
    dirty = true;
}

bool siren::Camera::is_dirty() const {
    return dirty;
}

siren::vec3 siren::Camera::get_position() const {
    return position;
}

void siren::Camera::set_position(siren::vec3 value) {
    position = value;
    dirty = true;
}

siren::vec3 siren::Camera::get_forward() const {
    return forward;
}

siren::vec3 siren::Camera::get_up() const {
    return up;
}

siren::vec3 siren::Camera::get_right() const {
    return vec3::cross(forward, up);
}

void siren::Camera::look_at(vec3 point) {
    // forward = position.direction_to(point);
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

        forward = vec3(
            cos(yaw_radians) * cos(pitch_radians),
            sin(pitch_radians),
            sin(yaw_radians) * cos(pitch_radians)
        ).normalized();
        // glm::vec3 right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
        // up = glm::cross(forward, up);
        view_matrix = mat4::look_at(position, position + forward, up);
        /*
        forward = vec3(
            cos(yaw_radians) * cos(pitch_radians),
            sin(pitch_radians),
            sin(yaw_radians) * cos(pitch_radians)
        ).normalized();
        view_matrix = mat4::look_at(position, position + forward, up);
        */

        dirty = false;
    }
    return view_matrix;
}