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
#include "texture.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace siren {
    struct Model {
        struct Mesh {
            uint32_t vao;
            uint32_t vbo;
            uint32_t ebo;
            uint32_t index_count;
            uint32_t index_offset;
            int index_component_type;

            Texture material_albedo;
            Texture material_metallic_roughness;
            Texture material_normal;
            Texture material_emissive;
            Texture material_occlusion;
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
            std::string name;
            float duration;
        };

        std::vector<Mesh> meshes;
        std::vector<Bone> bones;
        std::vector<Animation> animations;
        std::unordered_map<std::string, int> animation_id_lookup;
    };

    typedef uint32_t ModelHandle;
    static const ModelHandle MODEL_HANDLE_NULL = UINT32_MAX;

    SIREN_API ModelHandle model_acquire(const char* path);
    const Model& model_get(ModelHandle handle);

    class ModelTransform {
        public:
            static const int ANIMATION_NONE = -1;

            SIREN_API ModelTransform();
            SIREN_API ModelTransform(ModelHandle handle);

            SIREN_API const mat4& get_bone_transform(uint32_t index) const;

            SIREN_API std::string get_animation() const;
            SIREN_API void set_animation(std::string name, bool loop = false);
            SIREN_API void update_animation(float delta);

            Transform root;
        private:
            ModelHandle handle;
            std::vector<mat4> bone_transform;

            int animation;
            float animation_timer;
            bool animation_playing;
            bool animation_loop_on_finish;
    };
}