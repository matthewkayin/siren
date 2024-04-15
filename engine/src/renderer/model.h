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
        int parent_id;
        std::vector<std::vector<Transform>> keyframes;
        Transform initial_transform;
    };

    struct Model {
        std::vector<Mesh> mesh;
        std::vector<Bone> bones;
        std::unordered_map<std::string, int> bone_id_lookup;
        std::unordered_map<std::string, int> animation_id_lookup;
    };

    struct ModelTransform {
        static const int ANIMATION_NONE = -1;

        // TODO, change this to using uint32_t IDs as a model reference so that if a Model gets freed, we can check within the transform code that we are not referencing a dangling model
        Model* model;
        Transform root_transform;
        std::vector<Transform> bone_transform;

        int animation;
        uint32_t animation_frame;
        float animation_timer;
    };

    SIREN_API Model* model_acquire(const char* path);
    SIREN_API bool model_animation_add(Model* model, const char* name, const char* path);
    SIREN_API uint32_t model_animation_get_frame_count(Model* model, int animation_id);

    SIREN_API ModelTransform model_transform_create(Model* model);
    SIREN_API void model_transform_animation_set(ModelTransform* model_transform, const char* name);
    SIREN_API void model_transform_animation_update(ModelTransform* model_transform, float delta);

    struct ValveModelAcquireParams {
        const char* qc_path;
        const char* smd_path;
        const char* diffuse_path;
    };
    SIREN_API Model* valve_model_acquire(ValveModelAcquireParams params);
}