#version 410 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vec4 total_position = vec4(vertex_position, 1.0);
    gl_Position = projection * view * model * total_position;
}