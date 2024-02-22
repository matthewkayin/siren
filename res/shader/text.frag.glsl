#version 410 core

in vec2 frag_texture_coordinate;

out vec4 frag_color;

uniform sampler2D atlas_texture;
uniform vec3 text_color;

void main() {
    frag_color = vec4(text_color, texture(atlas_texture, frag_texture_coordinate).r);
}