#version 410 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texture_coordinate;

out vec2 frag_texture_coordinate;

void main() {
    gl_Position = vec4(in_position.x, in_position.y, 0.0, 1.0);
    frag_texture_coordinate = in_texture_coordinate;
}