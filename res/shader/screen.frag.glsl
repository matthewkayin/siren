#version 410 core

in vec2 frag_texture_coordinate;

out vec4 frag_color;

uniform sampler2D screen_texture;

void main() {
    frag_color = texture(screen_texture, frag_texture_coordinate);
}