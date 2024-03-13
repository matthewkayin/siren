#include "transform.h"

siren::mat4 siren::transform_to_matrix(const siren::Transform& transform) {
    return mat4::translate(transform.position) * 
            mat4::rotate(deg_to_rad(transform.rotation.x), VEC3_FORWARD) *
            mat4::rotate(deg_to_rad(transform.rotation.y), VEC3_RIGHT)   *
            mat4::rotate(deg_to_rad(transform.rotation.z), VEC3_UP) *
            mat4::scale(transform.scale);
}