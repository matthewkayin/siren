#pragma once

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/matrix.h"

namespace siren {
    typedef uint32_t Shader;

    bool shader_load(Shader* id, const char* vertex_path, const char* fragment_path);
    void shader_use(Shader id);
    void shader_set_uniform_int(Shader id, const char* name, int value);
    void shader_set_uniform_uint(Shader id, const char* name, uint32_t value);
    void shader_set_uniform_bool(Shader id, const char* name, bool value);
    void shader_set_uniform_float(Shader id, const char* name, float value);
    void shader_set_uniform_ivec2(Shader id, const char* name, ivec2 value);
    void shader_set_uniform_vec2(Shader id, const char* name, vec2 value);
    void shader_set_uniform_vec3(Shader id, const char* name, vec3 value);
    void shader_set_uniform_vec4(Shader id, const char* name, vec4 value);
    void shader_set_uniform_mat4(Shader id, const char* name, mat4* value, uint32_t size = 1);
}