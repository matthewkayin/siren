#pragma once

#include "defines.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "containers/darray.h"
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
        DArray<Mesh> mesh;
    };

    SIREN_API Model* model_acquire(const char* path);
}