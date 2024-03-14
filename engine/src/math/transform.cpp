#include "transform.h"

siren::mat4 siren::transform_to_matrix(const siren::Transform& transform) {
    return mat4::translate(transform.position) * transform.rotation.to_mat4() * mat4::scale(transform.scale);
}

/*
glm::mat4 siren::glm_transform_to_matrix(const siren::GlmTransform& transform) {
    glm::mat4 result = glm::mat4(1.0f);
    glm::translate(result, transform.position);
    glm::r
}
*/