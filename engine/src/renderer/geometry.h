#pragma once

#include "defines.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "texture.h"

namespace siren {
    struct Geometry {
        struct VertexData {
            vec3 position;
            vec3 normal;
            vec3 tex_coord;
        };

        uint32_t vao;
        uint32_t vbo;
        uint32_t vertex_count;

        Texture material_albedo;
    };

    Geometry geometry_create_cube(vec3 extents);
}