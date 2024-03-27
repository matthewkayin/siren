#version 410 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;
layout (location = 3) in ivec4 bone_ids;
layout (location = 4) in vec4 bone_weights;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture_coordinate;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 bone_matrix[MAX_BONES];

void main() {
    vec4 local_position = vec4(0.0);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (bone_ids[i] == -1) {
            continue;
        }

        if (bone_ids[i] > MAX_BONES - 1) {
            local_position = vec4(vertex_position, 1.0);
            break;
        }

        local_position += (bone_matrix[bone_ids[i]] * vec4(vertex_position, 1.0)) * bone_weights[i];
    }
    local_position = vec4(vertex_position, 1.0);

    gl_Position = projection * view * model * local_position;

    frag_position = vec3(model * local_position);
    // TODO pre-calc this before the shader
    frag_normal = normalize(mat3(transpose(inverse(model))) * normal);
    frag_texture_coordinate = texture_coordinate;
}