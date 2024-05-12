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

static std::vector<siren::Model> models;
static std::unordered_map<std::string, siren::ModelHandle> model_handles;

bool model_load(siren::Model* model, std::string path);

siren::ModelHandle siren::model_acquire(const char* path) {
    std::string key = std::string(path);
    auto it = model_handles.find(key);
    if (it != model_handles.end()) {
        return it->second;
    }

    Model model;
    if (!model_load(&model, resource_get_base_path() + path)) {
        return RESOURCE_HANDLE_NULL;
    }

    models.push_back(model);
    ModelHandle handle = models.size() - 1;
    model_handles[key] = handle;
    return handle;
}

const siren::Model& siren::model_get(siren::ModelHandle handle) {
    return models[handle];
}

uint32_t texture_create_from_glb(const tinygltf::Model& gltf_model, int texture_index) {
    const tinygltf::Texture& gltf_texture = gltf_model.textures[texture_index];
    const tinygltf::Image& image = gltf_model.images[gltf_texture.source];
    SIREN_TRACE("Loading glb texture %s...", image.name.c_str());

    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

    return texture;
}

bool model_load(siren::Model* model, std::string path) {
    SIREN_INFO("Loading model %s...", path.c_str());

    // Read the file
    tinygltf::TinyGLTF loader;
    std::string error;
    std::string warning;
    tinygltf::Model gltf_model;
    bool success = loader.LoadBinaryFromFile(&gltf_model, &error, &warning, path);
    if (!warning.empty()) {
        SIREN_WARN("%s", warning.c_str());
    }
    if (!error.empty()) {
        SIREN_ERROR("%s", error.c_str());
    }

    if (!success) {
        SIREN_ERROR("Failed to load glb file %s", path.c_str());
        return false;
    }

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

        // Add children to the stack
        for (uint32_t child_index = 0; child_index < node.children.size(); child_index++) {
            node_stack.push_back(node.children[child_index]);
        }
        
        // Skip the node if it doesn't have a mesh
        if (node.mesh < 0 || node.mesh > gltf_model.meshes.size() - 1) {
            continue;
        }

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
                    .bone_ids = { -1, -1, -1, -1 },
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
            // Albedo
            const tinygltf::Material& material = gltf_model.materials[primitive.material];
            if (material.pbrMetallicRoughness.baseColorTexture.index != -1) {
                mesh.material_albedo = texture_create_from_glb(gltf_model, material.pbrMetallicRoughness.baseColorTexture.index);
            } else {
                SIREN_ASSERT(material.pbrMetallicRoughness.baseColorFactor.size() != 0);
                mesh.material_albedo = siren::texture_acquire_solidcolor(
                    (uint8_t)(255.0f * material.pbrMetallicRoughness.baseColorFactor[0]),
                    (uint8_t)(255.0f * material.pbrMetallicRoughness.baseColorFactor[1]),
                    (uint8_t)(255.0f * material.pbrMetallicRoughness.baseColorFactor[2]),
                    255);
            } 
            // Metallic / Roughness
            if (material.pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
                mesh.material_metallic_roughness = texture_create_from_glb(gltf_model, material.pbrMetallicRoughness.metallicRoughnessTexture.index);
            } else {
                mesh.material_metallic_roughness = siren::texture_acquire_solidcolor(
                    0,
                    (uint8_t)(255.0f * material.pbrMetallicRoughness.roughnessFactor),
                    (uint8_t)(255.0f * material.pbrMetallicRoughness.metallicFactor),
                    0
                );
            }
            // Normal
            if (material.normalTexture.index != -1) {
                mesh.material_normal = texture_create_from_glb(gltf_model, material.normalTexture.index);
            } else {
                mesh.material_normal = siren::texture_acquire_solidcolor(128, 128, 255, 0);
            }
            // Emissive
            if (material.emissiveTexture.index != -1) {
                mesh.material_emissive = texture_create_from_glb(gltf_model, material.emissiveTexture.index);
            } else if (material.emissiveFactor.size() != 0) {
                mesh.material_emissive = siren::texture_acquire_solidcolor(
                    (uint8_t)(255.0f * material.emissiveFactor[0]),
                    (uint8_t)(255.0f * material.emissiveFactor[1]),
                    (uint8_t)(255.0f * material.emissiveFactor[2]),
                    0);
            } else {
                mesh.material_emissive = siren::texture_acquire_solidcolor(0, 0, 0, 0);
            }
            // Occlusion
            if (material.occlusionTexture.index != -1) {
                mesh.material_occlusion = texture_create_from_glb(gltf_model, material.occlusionTexture.index);
            } else {
                mesh.material_occlusion = siren::texture_acquire_solidcolor(255, 0, 0, 0);
            }

            model->meshes.push_back(mesh);
            SIREN_TRACE("Mesh added successfully.");
        } // End for each primitive

        // ~ Read the bones ~
        if (node.skin != -1) {
            const tinygltf::Skin& skin = gltf_model.skins[node.skin];
            const tinygltf::Accessor& inverse_bind_accessor = gltf_model.accessors[skin.inverseBindMatrices];
            const tinygltf::BufferView& inverse_bind_buffer_view = gltf_model.bufferViews[inverse_bind_accessor.bufferView];
            const tinygltf::Buffer& inverse_bind_buffer = gltf_model.buffers[inverse_bind_buffer_view.buffer];
            std::unordered_map<int, int> node_id_to_bone_id;
            for (int bone_index = 0; bone_index < skin.joints.size(); bone_index++) {
                const tinygltf::Node& bone_node = gltf_model.nodes[skin.joints[bone_index]];
                node_id_to_bone_id[skin.joints[bone_index]] = bone_index;

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
                    .transform = transform.to_mat4(),
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
            SIREN_TRACE("--- Model Bones ---");
            for (int bone_index = 0; bone_index < skin.joints.size(); bone_index++) {
                SIREN_TRACE("Bone %i name %s parent %i", bone_index, gltf_model.nodes[skin.joints[bone_index]].name.c_str(), model->bones[bone_index].parent_id);
            }

            // Import animations
            SIREN_TRACE("--- Model Animations ---");
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
                    .name = animation.name,
                    .duration = animation_duration,
                });
                model->animation_id_lookup[animation.name] = (int)animation_id;
            } // End for each animation
        } // End if node has skin
    } // End while not node stack empty

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    SIREN_INFO("glb loaded successfully.");
    return true;
}

siren::ModelTransform::ModelTransform() {
    handle = RESOURCE_HANDLE_NULL;
}

siren::ModelTransform::ModelTransform(siren::ModelHandle handle) {
    this->handle = handle;
    root = Transform::identity();

    const Model& model = model_get(handle);
    for (uint32_t bone_index = 0; bone_index < model.bones.size(); bone_index++) {
        bone_transform.push_back(model.bones[bone_index].transform);
    }

    animation = ModelTransform::ANIMATION_NONE;
    animation_timer = 0.0f;
    animation_loop_on_finish = false;
    animation_playing = false;
}

const siren::mat4& siren::ModelTransform::get_bone_transform(uint32_t index) const {
    return bone_transform[index];
}

std::string siren::ModelTransform::get_animation() const {
    const Model& model = model_get(handle);
    return model.animations[animation].name;
}

void siren::ModelTransform::set_animation(std::string name, bool loop) {
    const Model& model = model_get(handle);

    auto animation_id_it = model.animation_id_lookup.find(name);
    if (animation_id_it == model.animation_id_lookup.end()) {
        SIREN_WARN("called model_transform_set_animation() but animation '%s' does not exist on the model.", name.c_str());
        animation = -1;
        return;
    }

    animation = animation_id_it->second;
    animation_timer = 0.0f;
    animation_loop_on_finish = loop;
    animation_playing = true;
}

void siren::ModelTransform::update_animation(float delta) {
    const Model& model = model_get(handle);

    if (!animation_playing) {
        return;
    }

    // If no animation, set to bind pose
    if (animation == -1) {
        for (uint32_t bone_index = 0; bone_index < bone_transform.size(); bone_index++) {
            bone_transform[bone_index] = model.bones[bone_index].transform;
        }
        return;
    }

    // Increment timer and check if animation finished
    animation_timer += delta;
    if (animation_timer > model.animations[animation].duration) {
        if (!animation_loop_on_finish) {
            animation_playing = false;
            return;
        }

        animation_timer -= model.animations[animation].duration;
    }

    // Compute transforms for each bone
    for (uint32_t bone_index = 0; bone_index < bone_transform.size(); bone_index++) {
        const Keyframes& bone_keyframes = model.bones[bone_index].keyframes[animation];

        if (bone_keyframes.positions.size() == 0) {
            bone_transform[bone_index] = model.bones[bone_index].transform;
            continue;
        }

        vec3 bone_position;
        for (uint32_t position_index = 0; position_index < bone_keyframes.positions.size() - 1; position_index++) {
            if (animation_timer < bone_keyframes.positions[position_index + 1].time) {
                float percent = (animation_timer - bone_keyframes.positions[position_index].time) / (bone_keyframes.positions[position_index + 1].time - bone_keyframes.positions[position_index].time);
                bone_position = vec3::lerp(bone_keyframes.positions[position_index].value, bone_keyframes.positions[position_index + 1].value, percent);
                break;
            }
        }

        quat bone_rotation;
        for (uint32_t rotation_index = 0; rotation_index < bone_keyframes.rotations.size() - 1; rotation_index++) {
            if (animation_timer < bone_keyframes.rotations[rotation_index + 1].time) {
                float percent = (animation_timer - bone_keyframes.rotations[rotation_index].time) / (bone_keyframes.rotations[rotation_index + 1].time - bone_keyframes.rotations[rotation_index].time);
                bone_rotation = quat::slerp(bone_keyframes.rotations[rotation_index].value, bone_keyframes.rotations[rotation_index + 1].value, percent);
                break;
            }
        }

        vec3 bone_scale;
        for (uint32_t scale_index = 0; scale_index < bone_keyframes.scales.size() - 1; scale_index++) {
            if (animation_timer < bone_keyframes.scales[scale_index + 1].time) {
                float percent = (animation_timer - bone_keyframes.scales[scale_index].time) / (bone_keyframes.scales[scale_index + 1].time - bone_keyframes.scales[scale_index].time);
                bone_scale = vec3::lerp(bone_keyframes.scales[scale_index].value, bone_keyframes.scales[scale_index + 1].value, percent);
                break;
            }
        }

        bone_transform[bone_index] = ((Transform) {
            .position = bone_position,
            .rotation = bone_rotation,
            .scale = bone_scale
        }).to_mat4();
    } // End for each bone
}