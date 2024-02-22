#version 410 core

layout (location = 0) in vec2 vertex_position;

uniform vec2 screen_size;
uniform sampler2D atlas_texture;

uniform vec2 render_position;
uniform vec2 glyph_size;
uniform vec2 glyph_offset;

out vec2 frag_texture_coordinate;

void main() {
    vec2 position = render_position + vec2(vertex_position.x * glyph_size.x, vertex_position.y * glyph_size.y);
    vec2 screen_position = (2.0 * vec2(position.x / screen_size.x, position.y / screen_size.y)) - vec2(1.0, 1.0); 
    gl_Position = vec4(screen_position.x, screen_position.y, 0.0, 1.0);

    vec2 texture_size = vec2(textureSize(atlas_texture, 0));
    frag_texture_coordinate = glyph_offset + vec2(vertex_position.x * glyph_size.x, vertex_position.y * glyph_size.y);
    frag_texture_coordinate = vec2(frag_texture_coordinate.x / texture_size.x, 1.0 - ((texture_size.y - frag_texture_coordinate.y) / texture_size.y));
}