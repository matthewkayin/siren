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

    typedef uint32_t ModelHandle;
    static const ModelHandle MODEL_HANDLE_NULL = UINT32_MAX;

    SIREN_API ModelHandle model_acquire(const char* path);
    const Model& model_get(ModelHandle handle);

    struct ModelTransform {
        static const int ANIMATION_NONE = -1;

        SIREN_API ModelTransform();
        SIREN_API ModelTransform(ModelHandle handle);
        SIREN_API void animation_set(std::string name);
        SIREN_API void animation_update(float delta);

        ModelHandle handle;
        Transform root_transform;
        std::vector<mat4> bone_transform;

        int animation;
        float animation_timer;
    };
}