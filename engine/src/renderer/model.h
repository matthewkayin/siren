#pragma once

#include "defines.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "renderer/texture.h"

namespace siren {
    struct Mesh {
        uint32_t vao;
        uint32_t vbo;
        uint32_t ebo;
        uint32_t index_count;
        vec3 offset;
        Texture map_diffuse;
    };

    struct Model {
        Mesh* mesh;
        uint32_t mesh_count;
    };

    SIREN_API bool model_load(Model* model, const char* path);
    SIREN_API void model_free(Model* model);
}