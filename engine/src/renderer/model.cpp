#include "model.h"

#include "core/logger.h"
#include "core/resource.h"
#include "core/asserts.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include <tiny_gltf.h>
#include <glad/glad.h>
#include <vector>
#include <unordered_map>
#include <fstream>


static std::unordered_map<std::string, siren::Model> models;

bool model_load(siren::Model* model, const char* path);

siren::Model* siren::model_acquire(const char* path) {
    std::string key = std::string(path);
    auto it = models.find(key);
    if (it != models.end()) {
        return &it->second;
    }

    Model model;
    if (!model_load(&model, path)) {
        return nullptr;
    }

    models[key] = model;
    return &models[key];
}

bool model_load(siren::Model* model, const char* path) {
    std::string full_path = std::string(siren::resource_get_base_path()) + std::string(path);

    SIREN_INFO("Loading glb %s...", full_path.c_str());

    // Read the file
    tinygltf::TinyGLTF loader;
    std::string error;
    std::string warning;
    tinygltf::Model gltf_model;
    bool success = loader.LoadBinaryFromFile(&gltf_model, &error, &warning, full_path);
    if (!warning.empty()) {
        SIREN_WARN("%s", warning.c_str());
    }
    if (!error.empty()) {
        SIREN_ERROR("%s", error.c_str());
    }

    if (!success) {
        SIREN_ERROR("Failed to load glb file %s", path);
        return false;
    }

    SIREN_TRACE("Opened glb file %s", path);

    // Create meshes
    const tinygltf::Scene& scene = gltf_model.scenes[gltf_model.defaultScene];
    std::vector<int> node_stack;
    for (uint32_t node_index = 0; node_index < scene.nodes.size(); node_index++) {
        node_stack.push_back(scene.nodes[node_index]);
    }

    while (!node_stack.empty()) {
        // Get the node on top of the stack
        int node_index = node_stack[node_stack.size() - 1];
        node_stack.pop_back();
        SIREN_ASSERT(node_index >= 0 && node_index < gltf_model.nodes.size());
        tinygltf::Node& node = gltf_model.nodes[node_index];
        SIREN_TRACE("Handling node with index %i and name %s", node_index, node.name.c_str());

        // Add children to the stack
        for (uint32_t child_index = 0; child_index < node.children.size(); child_index++) {
            node_stack.push_back(node.children[child_index]);
        }
        
        // Skip the node if it doesn't have a mesh
        if (node.mesh < 0 || node.mesh > gltf_model.meshes.size() - 1) {
            continue;
        }

        SIREN_TRACE("Node has mesh. Looping through primitives.");
        // Create a mesh for this node
        const tinygltf::Mesh& gltf_mesh = gltf_model.meshes[node.mesh];
        for (uint32_t primitive_index = 0; primitive_index < gltf_mesh.primitives.size(); primitive_index++) {
            // Setup the siren mesh
            SIREN_TRACE("Setting up the mesh for primitive %u...", primitive_index);
            siren::Mesh mesh;
            glGenVertexArrays(1, &mesh.vao);
            glBindVertexArray(mesh.vao);

            const tinygltf::Primitive& primitive = gltf_mesh.primitives[primitive_index];
            std::vector<siren::vec3> positions;
            std::vector<siren::vec3> normals;
            std::vector<siren::vec2> tex_coords;
            std::vector<std::vector<uint8_t>> bone_ids;
            std::vector<std::vector<float>> bone_weights;
            for (auto& attribute : primitive.attributes) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[attribute.second];
                const tinygltf::BufferView& buffer_view = gltf_model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = gltf_model.buffers[buffer_view.buffer];

                if (attribute.first == "POSITION") {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * sizeof(siren::vec3)); i += sizeof(siren::vec3)) {
                        siren::vec3 v;
                        std::memcpy(&v, &buffer.data.at(i), sizeof(siren::vec3));
                        positions.push_back(v);
                    }
                } else if (attribute.first == "NORMAL") {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * sizeof(siren::vec3)); i += sizeof(siren::vec3)) {
                        siren::vec3 v;
                        std::memcpy(&v, &buffer.data.at(i), sizeof(siren::vec3));
                        normals.push_back(v);
                    }
                } else if (attribute.first == "TEXCOORD_0") {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * sizeof(siren::vec2)); i += sizeof(siren::vec2)) {
                        siren::vec2 v;
                        std::memcpy(&v, &buffer.data.at(i), sizeof(siren::vec2));
                        tex_coords.push_back(v);
                    }
                } else if (attribute.first == "JOINTS_0") {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * 4 * sizeof(uint8_t)); i += 4 * sizeof(uint8_t)) {
                        std::vector<uint8_t> value(4, 0);
                        std::memcpy(&value[0], &buffer.data.at(i), 4 * sizeof(unsigned char));
                        bone_ids.push_back(value);
                    }
                } else if (attribute.first == "WEIGHTS_0") {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * 4 * sizeof(float)); i += 4 * sizeof(float)) {
                        std::vector<float> value(4, 0.0f);
                        std::memcpy(&value[0], &buffer.data.at(i), 4 * sizeof(float));
                        bone_weights.push_back(value);
                    }
                } else {
                    SIREN_WARN("Unhandled vertex array attribute %s. Skipping...", attribute.first.c_str());
                    continue;
                }
            } // End for each primitive attribute
            struct VertexData {
                siren::vec3 position;
                siren::vec3 normal;
                siren::vec2 tex_coord;
                int bone_ids[4];
                float bone_weights[4];
            };
            std::vector<VertexData> vertex_data;
            for (uint32_t i = 0; i < positions.size(); i++) {
                vertex_data.push_back((VertexData) {
                    .position = positions[i],
                    .normal = normals[i],
                    .tex_coord = tex_coords[i],
                    .bone_ids = { 0, 0, 0, 0 },
                    .bone_weights = { 0.0f, 0.0f, 0.0f, 0.0f }
                });
                // Making this check because bone_ids are optional on a model
                // Note that bone_ids will still be in the vertex data, they will just have -1 values so that they won't be used
                if (bone_ids.size() != 0) {
                    for (uint32_t b = 0; b < 4; b++) {
                        vertex_data[vertex_data.size() - 1].bone_ids[b] = bone_ids[i][b];
                        vertex_data[vertex_data.size() - 1].bone_weights[b] = bone_weights[i][b];
                    }
                }
            }
            glGenBuffers(1, &mesh.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(3);
            glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexData), (void*)offsetof(VertexData, bone_ids));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, bone_weights));

            // Buffer the indices
            const tinygltf::Accessor& index_accessor = gltf_model.accessors[primitive.indices];
            const tinygltf::BufferView& index_buffer_view = gltf_model.bufferViews[index_accessor.bufferView];
            const tinygltf::Buffer& index_buffer = gltf_model.buffers[index_buffer_view.buffer];
            mesh.index_count = index_accessor.count;
            mesh.index_offset = index_accessor.byteOffset;
            mesh.index_component_type = index_accessor.componentType;
            glGenBuffers(1, &mesh.ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_view.byteLength, &index_buffer.data.at(0) + index_buffer_view.byteOffset, GL_STATIC_DRAW);

            // Setup the siren material
            const tinygltf::Material& material = gltf_model.materials[primitive.material];
            const tinygltf::Texture& texture = gltf_model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
            const tinygltf::Image& image = gltf_model.images[texture.source];
            SIREN_TRACE("Loading texture %i with source %i...", material.pbrMetallicRoughness.baseColorTexture.index, texture.source);

            glGenTextures(1, &mesh.texture_diffuse);
            glBindTexture(GL_TEXTURE_2D, mesh.texture_diffuse);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            GLenum format = GL_RGBA;
            if (image.component == 1) {
                format = GL_RED;
            } else if (image.component == 2) {
                format = GL_RG;
            } else if (image.component == 3) {
                format = GL_RGB;
            } 

            GLenum type = GL_UNSIGNED_BYTE;
            if (image.bits == 16) {
                type = GL_UNSIGNED_SHORT;
            } 

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, format, type, &image.image.at(0));
            SIREN_TRACE("Texture loaded successfully.");

            model->meshes.push_back(mesh);
            SIREN_TRACE("Mesh added successfully.");
        } // End for each primitive

        // ~ Read the bones ~
        const tinygltf::Skin& skin = gltf_model.skins[node.skin];
        const tinygltf::Accessor& inverse_bind_accessor = gltf_model.accessors[skin.inverseBindMatrices];
        const tinygltf::BufferView& inverse_bind_buffer_view = gltf_model.bufferViews[inverse_bind_accessor.bufferView];
        const tinygltf::Buffer& inverse_bind_buffer = gltf_model.buffers[inverse_bind_buffer_view.buffer];
        std::unordered_map<int, int> node_id_to_bone_id;
        for (int bone_index = 0; bone_index < skin.joints.size(); bone_index++) {
            const tinygltf::Node& bone_node = gltf_model.nodes[skin.joints[bone_index]];
            node_id_to_bone_id[skin.joints[bone_index]] = bone_index;
            SIREN_TRACE("bone id %u node id %i name %s", bone_index, skin.joints[bone_index], bone_node.name.c_str());

            siren::Transform transform = (siren::Transform) {
                .position = siren::vec3(0.0f),
                .rotation = siren::quat(),
                .scale = siren::vec3(1.0f)
            };
            if (bone_node.translation.size() != 0) {
                transform.position = siren::vec3(bone_node.translation.at(0), bone_node.translation.at(1), bone_node.translation.at(2));
            }
            if (bone_node.rotation.size() != 0) {
                transform.rotation = siren::quat(bone_node.rotation.at(0), bone_node.rotation.at(1), bone_node.rotation.at(2), bone_node.rotation.at(3));
            }
            if (bone_node.scale.size() != 0) {
                transform.scale = siren::vec3(bone_node.scale.at(0), bone_node.scale.at(1), bone_node.scale.at(2));
            }

            siren::mat4 inverse_bind_transform;
            memcpy(&inverse_bind_transform[0], &inverse_bind_buffer.data.at(0) + inverse_bind_buffer_view.byteOffset + (bone_index * sizeof(siren::mat4)), sizeof(siren::mat4));

            model->bones.push_back((siren::Bone) {
                .parent_id = -1,
                .keyframes = std::vector<siren::Keyframes>(),
                .transform = siren::transform_to_matrix(transform),
                // .transform = siren::mat4(1.0f),
                .inverse_bind_transform = inverse_bind_transform
            });
        } // End for each bone index
        // Determine bone parents
        for (int bone_index = 0; bone_index < skin.joints.size(); bone_index++) {
            const tinygltf::Node& bone_node = gltf_model.nodes[skin.joints[bone_index]];
            for (uint32_t child_index = 0; child_index < bone_node.children.size(); child_index++) {
                int child_bone_id = node_id_to_bone_id[bone_node.children[child_index]];
                model->bones[child_bone_id].parent_id = bone_index;
            }
        }
        for (int bone_index = 0; bone_index < skin.joints.size(); bone_index++) {
            SIREN_TRACE("bone %i name %s parent %i", bone_index, gltf_model.nodes[skin.joints[bone_index]].name.c_str(), model->bones[bone_index].parent_id);
        }

        // Import animations
        for (uint32_t animation_index = 0; animation_index < gltf_model.animations.size(); animation_index++) {
            const tinygltf::Animation& animation = gltf_model.animations[animation_index];

            uint32_t animation_id = model->bones[0].keyframes.size();
            for (uint32_t bone_index = 0; bone_index < model->bones.size(); bone_index++) {
                model->bones[bone_index].keyframes.push_back(siren::Keyframes());
            }

            float animation_duration = 0.0f;
            SIREN_TRACE("Animation %u: %s", animation_index, animation.name.c_str());

            for (uint32_t channel_index = 0; channel_index < animation.channels.size(); channel_index++) {
                const tinygltf::AnimationChannel& channel = animation.channels[channel_index];
                const tinygltf::AnimationSampler& sampler = animation.samplers[channel.sampler];

                const tinygltf::Accessor& time_accessor = gltf_model.accessors[sampler.input];
                const tinygltf::BufferView& time_buffer_view = gltf_model.bufferViews[time_accessor.bufferView];
                const tinygltf::Buffer& time_buffer = gltf_model.buffers[time_buffer_view.buffer];

                const tinygltf::Accessor& value_accessor = gltf_model.accessors[sampler.output];
                const tinygltf::BufferView& value_buffer_view = gltf_model.bufferViews[value_accessor.bufferView];
                const tinygltf::Buffer& value_buffer = gltf_model.buffers[value_buffer_view.buffer];

                SIREN_ASSERT(node_id_to_bone_id.find(channel.target_node) != node_id_to_bone_id.end());
                int bone_id = node_id_to_bone_id[channel.target_node];
                for (uint32_t i = 0; i < time_accessor.count; i++) {
                    if (channel.target_path == "translation") {
                        siren::KeyframeVec3 keyframe;
                        memcpy(&keyframe.time, &time_buffer.data.at(0) + time_buffer_view.byteOffset + (i * sizeof(float)), sizeof(float));
                        memcpy(&keyframe.value, &value_buffer.data.at(0) + value_buffer_view.byteOffset + (i * sizeof(siren::vec3)), sizeof(siren::vec3));
                        animation_duration = siren::fmax(animation_duration, keyframe.time);
                        model->bones[bone_id].keyframes[animation_id].positions.push_back(keyframe);
                    } else if (channel.target_path == "rotation") {
                        siren::KeyframeQuat keyframe;
                        memcpy(&keyframe.time, &time_buffer.data.at(0) + time_buffer_view.byteOffset + (i * sizeof(float)), sizeof(float));
                        memcpy(&keyframe.value, &value_buffer.data.at(0) + value_buffer_view.byteOffset + (i * sizeof(siren::quat)), sizeof(siren::quat));
                        animation_duration = siren::fmax(animation_duration, keyframe.time);
                        model->bones[bone_id].keyframes[animation_id].rotations.push_back(keyframe);
                    } else if (channel.target_path == "scale") {
                        siren::KeyframeVec3 keyframe;
                        memcpy(&keyframe.time, &time_buffer.data.at(0) + time_buffer_view.byteOffset + (i * sizeof(float)), sizeof(float));
                        memcpy(&keyframe.value, &value_buffer.data.at(0) + value_buffer_view.byteOffset + (i * sizeof(siren::vec3)), sizeof(siren::vec3));
                        animation_duration = siren::fmax(animation_duration, keyframe.time);
                        model->bones[bone_id].keyframes[animation_id].scales.push_back(keyframe);
                    }
                } // End for each keyframe in channel
            } // End for each animation channel

            model->animations.push_back((siren::Animation) {
                .duration = animation_duration,
                .ticks_per_second = 1.0
            });
            model->animation_id_lookup[animation.name] = (int)animation_id;
        }
    } // End while not node stack empty

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    SIREN_INFO("glb loaded successfully.");
    return true;
}

siren::ModelTransform siren::model_transform_create(siren::Model* model) {
    ModelTransform result;

    result.model = model;
    result.root_transform = transform_identity();

    std::vector<mat4> bone_transform;
    for (uint32_t bone_index = 0; bone_index < model->bones.size(); bone_index++) {
        // bone_transform.push_back(mat4(1.0f));
        bone_transform.push_back(model->bones[bone_index].transform);
    }
    result.bone_transform = bone_transform;

    result.animation = ModelTransform::ANIMATION_NONE;
    result.animation_timer = 0.0f;

    return result;
}

void siren::model_transform_animation_set(ModelTransform* model_transform, const char* name) {
    auto animation_id_it = model_transform->model->animation_id_lookup.find(std::string(name));
    if (animation_id_it == model_transform->model->animation_id_lookup.end()) {
        SIREN_WARN("called model_transform_set_animation() but animation '%s' does not exist on the model.", name);
        return;
    }

    model_transform->animation = animation_id_it->second;
    SIREN_TRACE("set to animation id %i", model_transform->animation);
    model_transform->animation_timer = 0.0f;
}

void siren::model_transform_animation_update(ModelTransform* model_transform, float delta) {
    Model* model = model_transform->model;
    int animation_id = model_transform->animation;

    model_transform->animation_timer += delta;
    // Loops animation
    if (model_transform->animation_timer > model->animations[animation_id].duration) {
        model_transform->animation_timer -= model->animations[animation_id].duration;
    }

    for (uint32_t bone_index = 0; bone_index < model_transform->bone_transform.size(); bone_index++) {
        Keyframes& bone_keyframes = model->bones[bone_index].keyframes[animation_id];

        if (bone_keyframes.positions.size() == 0) {
            model_transform->bone_transform[bone_index] = model->bones[bone_index].transform;
            continue;
        }

        vec3 bone_position;
        for (uint32_t position_index = 0; position_index < bone_keyframes.positions.size() - 1; position_index++) {
            if (model_transform->animation_timer < bone_keyframes.positions[position_index + 1].time) {
                float percent = (model_transform->animation_timer - bone_keyframes.positions[position_index].time) / (bone_keyframes.positions[position_index + 1].time - bone_keyframes.positions[position_index].time);
                bone_position = vec3::lerp(bone_keyframes.positions[position_index].value, bone_keyframes.positions[position_index + 1].value, percent);
                break;
            }
        }

        quat bone_rotation;
        for (uint32_t rotation_index = 0; rotation_index < bone_keyframes.rotations.size() - 1; rotation_index++) {
            if (model_transform->animation_timer < bone_keyframes.rotations[rotation_index + 1].time) {
                float percent = (model_transform->animation_timer - bone_keyframes.rotations[rotation_index].time) / (bone_keyframes.rotations[rotation_index + 1].time - bone_keyframes.rotations[rotation_index].time);
                bone_rotation = quat::slerp(bone_keyframes.rotations[rotation_index].value, bone_keyframes.rotations[rotation_index + 1].value, percent);
                break;
            }
        }

        vec3 bone_scale;
        for (uint32_t scale_index = 0; scale_index < bone_keyframes.scales.size() - 1; scale_index++) {
            if (model_transform->animation_timer < bone_keyframes.scales[scale_index + 1].time) {
                float percent = (model_transform->animation_timer - bone_keyframes.scales[scale_index].time) / (bone_keyframes.scales[scale_index + 1].time - bone_keyframes.scales[scale_index].time);
                bone_scale = vec3::lerp(bone_keyframes.scales[scale_index].value, bone_keyframes.scales[scale_index + 1].value, percent);
                break;
            }
        }

        model_transform->bone_transform[bone_index] = transform_to_matrix((Transform) {
            .position = bone_position,
            .rotation = bone_rotation,
            .scale = bone_scale
        });
    }
}