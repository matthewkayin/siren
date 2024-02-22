#pragma once

#include "math/math.h"

#include <glad/glad.h>

namespace siren {
    typedef GLuint Shader;

    bool shader_load(Shader* id, const char* vertex_path, const char* fragment_path);
    void shader_use(Shader id);
    void shader_set_uniform_int(Shader id, const char* name, int value);
    void shader_set_uniform_uint(Shader id, const char* name, uint32_t value);
    void shader_set_uniform_bool(Shader id, const char* name, bool value);
    void shader_set_uniform_ivec2(Shader id, const char* name, ivec2 value);
    void shader_set_uniform_vec2(Shader id, const char* name, vec2 value);
    void shader_set_uniform_vec3(Shader id, const char* name, vec3 value);
    void shader_set_uniform_mat4(Shader id, const char* name, mat4 value);
}