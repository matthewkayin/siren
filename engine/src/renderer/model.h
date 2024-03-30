#pragma once

#ifndef SIREN_MAX_BONE_INFLUENCE
#define SIREN_MAX_BONE_INFLUENCE 4
#endif 

#include "defines.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix.h"
#include "math/transform.h"
#include "renderer/texture.h"

#include <vector>
#include <unordered_map>
#include <string>

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
        Transform transform;
        std::vector<std::vector<Transform>> keyframes;
        mat4 offset;
    };

    struct Model {
        std::vector<Mesh> mesh;
        std::vector<Bone> bones;
        std::unordered_map<std::string, int> bone_id_lookup;
        std::unordered_map<std::string, int> animation_id_lookup;
    };

    SIREN_API Model* model_acquire(const char* path);
    SIREN_API void model_add_animation(Model* model, const char* path);
}