#pragma once

#include "defines.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "containers/darray.h"
#include "renderer/texture.h"

namespace siren {
    struct Mesh {
        uint32_t vao;
        uint32_t vbo;
        uint32_t ebo;
        uint32_t index_count;
        vec3 offset;

        vec3 color_ambient;
        vec3 color_diffuse;
        vec3 color_specular;
        float shininess;
        float shininess_strength;
        Texture texture_diffuse;
        Texture texture_emissive;
    };

    struct Model {
        DArray<Mesh> mesh;
    };

    SIREN_API Model* model_acquire(const char* path);
}