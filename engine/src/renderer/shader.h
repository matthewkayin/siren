#pragma once

#include "math/math.h"

#include <glad/glad.h>

typedef GLuint Shader;

namespace siren {
    bool shader_load(Shader* id, const char* vertex_path, const char* fragment_path);
    void shader_use(Shader id);
    void shader_set_uniform(Shader id, const char* name, int value);
    void shader_set_uniform(Shader id, const char* name, uint32_t value);
    void shader_set_uniform(Shader id, const char* name, bool value);
    void shader_set_uniform(Shader id, const char* name, ivec2 value);
    void shader_set_uniform(Shader id, const char* name, vec2 value);
    void shader_set_uniform(Shader id, const char* name, vec3 value);
    void shader_set_uniform(Shader id, const char* name, mat4 value);
}