#pragma once

#ifndef SIREN_MAX_BONE_INFLUENCE
#define SIREN_MAX_BONE_INFLUENCE 4
#endif 

#include "defines.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix.h"
#include "containers/darray.h"
#include "containers/hashtable.h"
#include "renderer/texture.h"

namespace siren {
    struct Mesh {
        uint32_t vao;
        uint32_t vbo;
        uint32_t ebo;
        uint32_t index_count;

        vec3 color_ambient;
        vec3 color_diffuse;
        vec3 color_specular;
        float shininess;
        float shininess_strength;
        Texture texture_diffuse;
        Texture texture_emissive;
    };

    struct Bone {
        int child_ids[4];
        mat4 offset;
    };

    struct Model {
        DArray<Mesh> mesh;
        DArray<Bone> bones;
        Hashtable<int> bone_id_lookup;
    };

    SIREN_API Model* model_acquire(const char* path);
}