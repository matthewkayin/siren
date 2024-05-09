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
        uint32_t index_offset;
        int index_component_type;

        Texture texture_diffuse;
    };

    struct KeyframeVec3 {
        vec3 value;
        float time;
    };

    struct KeyframeQuat {
        quat value;
        float time;
    };

    struct Keyframes {
        std::vector<KeyframeVec3> positions;
        std::vector<KeyframeQuat> rotations;
        std::vector<KeyframeVec3> scales;
    };

    struct Bone {
        int parent_id;
        std::vector<Keyframes> keyframes;
        mat4 transform;
        mat4 inverse_bind_transform;
    };

    struct Animation {
        float duration;
        float ticks_per_second;
    };

    struct Model {
        std::vector<Mesh> meshes;
        std::vector<Bone> bones;
        std::vector<Animation> animations;
        std::unordered_map<std::string, int> bone_id_lookup;
        std::unordered_map<std::string, int> animation_id_lookup;
    };

    struct ModelTransform {
        static const int ANIMATION_NONE = -1;

        // TODO, change this to using uint32_t IDs as a model reference so that if a Model gets freed, we can check within the transform code that we are not referencing a dangling model
        Model* model;
        Transform root_transform;
        std::vector<mat4> bone_transform;

        int animation;
        float animation_timer;
    };

    SIREN_API Model* model_acquire(const char* path);

    SIREN_API ModelTransform model_transform_create(Model* model);
    SIREN_API void model_transform_animation_set(ModelTransform* model_transform, const char* name);
    SIREN_API void model_transform_animation_update(ModelTransform* model_transform, float delta);
}