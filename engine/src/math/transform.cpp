#include "transform.h"

siren::Transform::Transform() {
    position = vec3();
    rotation = quat();
    scale = vec3(1.0f);

    local = mat4::identity();
    is_dirty = true;
    parent = nullptr;
}

siren::Transform::Transform(vec3 position) {
    this->position = position;
    rotation = quat();
    scale = vec3(1.0f);

    local = mat4::identity();
    is_dirty = true;
    parent = nullptr;
}

siren::Transform::Transform(quat rotation) {
    position = vec3();
    this->rotation = rotation;
    scale = vec3(1.0f);

    local = mat4::identity();
    is_dirty = true;
    parent = nullptr;
}

siren::Transform::Transform(vec3 position, quat rotation) {
    this->position = position;
    this->rotation = rotation;
    scale = vec3(1.0f);

    local = mat4::identity();
    is_dirty = true;
    parent = nullptr;
}

siren::Transform::Transform(vec3 position, quat rotation, vec3 scale) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;

    local = mat4::identity();
    is_dirty = true;
    parent = nullptr;
}

void siren::Transform::set_position(vec3 position) {
    this->position = position;
    is_dirty = true;
}

void siren::Transform::set_rotation(quat rotation) {
    this->rotation = rotation;
    is_dirty = true;
}

void siren::Transform::set_scale(vec3 scale) {
    this->scale = scale;
    is_dirty = true;
}

siren::mat4 siren::Transform::get_matrix_local() {
    if (is_dirty) {
        mat4 _scale = mat4::scale(scale);
        mat4 _rotation = rotation.to_mat4();
        mat4 _translation = mat4::translate(position);
        mat4 _rot_trans = _rotation * _translation;
        local = mat4::scale(scale) * (rotation.to_mat4() * mat4::translate(position)); 
        quat _normal = rotation.normalized();
        is_dirty = false;
    }
    return local;
}

siren::mat4 siren::Transform::get_matrix_world() {
    mat4 world = get_matrix_local();
    if (parent != nullptr) {
        world = world * parent->get_matrix_world();
    }

    determinant = world.determinant();
    return world;
}