#version 410 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texture_coordinate;

out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_texture_coordinate;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vec4 total_position = vec4(vertex_position, 1.0);
    gl_Position = projection * view * model * total_position;

    frag_position = vec3(model * total_position);
    // TODO pre-calc this before the shader
    frag_normal = normalize(mat3(transpose(inverse(model))) * normal);
    frag_texture_coordinate = texture_coordinate;
}