#include "model.h"

#include "core/logger.h"
#include "core/resource.h"
#include "core/asserts.h"

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <unordered_map>
#include <fstream>

static std::unordered_map<std::string, siren::Model> models;

siren::mat4 assimp_mat4_to_siren_mat4(aiMatrix4x4 m) {
    siren::mat4 result;

    result[0][0] = m.a1;
    result[1][0] = m.a2;
    result[2][0] = m.a3;
    result[3][0] = m.a4;
    
    result[0][1] = m.b1;
    result[1][1] = m.b2;
    result[2][1] = m.b3;
    result[3][1] = m.b4;

    result[0][2] = m.c1;
    result[1][2] = m.c2;
    result[2][2] = m.c3;
    result[3][2] = m.c4;

    result[0][3] = m.d1;
    result[1][3] = m.d2;
    result[2][3] = m.d3;
    result[3][3] = m.d4;

    return result;
}

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

/*
bool model_load(siren::Model* model, const char* path) {
    std::string short_path = std::string(path);
    std::string full_path = std::string(siren::resource_get_base_path()) + short_path;
    std::string short_path_directory = short_path.substr(0, short_path.find_last_of("/") + 1);

    SIREN_TRACE("Loading model %s...", full_path.c_str());

    // Load the model file
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(full_path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        SIREN_ERROR("Error loading model: %s", importer.GetErrorString());
        return false;
    }

    // Collect all the meshes from the scene hierarchy 
    std::vector<aiMesh*> meshes;
    std::vector<aiNode*> node_stack;
    node_stack.push_back(scene->mRootNode);

    while (!node_stack.empty()) {
        aiNode* current = node_stack[node_stack.size() - 1];
        node_stack.pop_back();

        for (uint32_t i = 0; i < current->mNumMeshes; i++) {
            meshes.push_back(scene->mMeshes[current->mMeshes[i]]);
        }
        for (uint32_t i = 0; i < current->mNumChildren; i++) {
            node_stack.push_back(current->mChildren[i]);
        }
    }


    // Initialize mesh array
    model->mesh.reserve(meshes.size());

    // Create each mesh using the loaded mesh data and buffer its data to the gpu
    struct VertexData {
        siren::vec3 position;
        siren::vec3 normal;
        siren::vec2 texture_coordinate;
        int bone_ids[SIREN_MAX_BONE_INFLUENCE];
        float bone_weights[SIREN_MAX_BONE_INFLUENCE];
    };
    for (uint32_t i = 0; i < meshes.size(); i++) {
        SIREN_TRACE("Creating mesh index %u, name %s... ", i, meshes[i]->mName.C_Str());
        // collect mesh vertices
        std::vector<VertexData> vertices;
        for (uint32_t v = 0; v < meshes[i]->mNumVertices; v++) {
            // Note: assimp models might have multiple texture coordinates. currently I'm naively assuming that we can just use coordinate set 0,
            // but later on I might need to write some code which decides which coordinates to use
            vertices.push_back((VertexData) {
                .position = siren::vec3(meshes[i]->mVertices[v].x, meshes[i]->mVertices[v].y, meshes[i]->mVertices[v].z),
                .normal = siren::vec3(meshes[i]->mNormals[v].x, meshes[i]->mNormals[v].y, meshes[i]->mNormals[v].z),
                .texture_coordinate = siren::vec2(meshes[i]->mTextureCoords[0][v].x, meshes[i]->mTextureCoords[0][v].y),
                .bone_ids = { -1, -1, -1, -1 },
                .bone_weights = { 0.0f, 0.0f, 0.0f, 0.0f }
            });
        }

        // collect mesh indices
        std::vector<uint32_t> indices;
        for (uint32_t f = 0; f < meshes[i]->mNumFaces; f++) {
            aiFace face = meshes[i]->mFaces[f];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // ~ Read the bones ~
        for (uint32_t assimp_bone_index = 0; assimp_bone_index < meshes[i]->mNumBones; assimp_bone_index++) {
            int bone_id = -1;

            // Create the bones 
            std::string bone_name = std::string(meshes[i]->mBones[assimp_bone_index]->mName.C_Str());
            auto bone_id_it = model->bone_id_lookup.find(bone_name);
            if (bone_id_it == model->bone_id_lookup.end()) {
                siren::Bone new_bone = (siren::Bone) {
                    .parent_id = -1,
                    .keyframes = std::vector<siren::Keyframes>(),
                    // .transform = assimp_mat4_to_siren_mat4(meshes[i]->mBones[assimp_bone_index]->mNode->mTransformation),
                    .transform = siren::mat4(1.0f),
                    .inverse_bind_transform = assimp_mat4_to_siren_mat4(meshes[i]->mBones[assimp_bone_index]->mOffsetMatrix)
                };
                model->bones.push_back(new_bone);
                bone_id = model->bones.size() - 1;
                model->bone_id_lookup[bone_name] = bone_id;
            } else {
                bone_id = bone_id_it->second;
            }

            SIREN_ASSERT(bone_id != -1);

            // Add the bone weights to the vertex data
            aiVertexWeight* weights = meshes[i]->mBones[assimp_bone_index]->mWeights;
            int num_weights = meshes[i]->mBones[assimp_bone_index]->mNumWeights;
            for (uint32_t weight_index = 0; weight_index < num_weights; weight_index++) {
                int vertex_id = weights[weight_index].mVertexId;
                float bone_weight = weights[weight_index].mWeight;

                SIREN_ASSERT(vertex_id < vertices.size());

                // Place the bone data in an empty slot in the vertex's bone array
                for (uint32_t vertex_bone_index = 0; vertex_bone_index < SIREN_MAX_BONE_INFLUENCE; vertex_bone_index++) {
                    if (vertices[vertex_id].bone_ids[vertex_bone_index] == -1) {
                        vertices[vertex_id].bone_ids[vertex_bone_index] = bone_id;
                        vertices[vertex_id].bone_weights[vertex_bone_index] = bone_weight;
                        break;
                    }
                } // End for each vertex bone index
            } // End for each bone weight
        } // End for each bone

        // Traverse the node stack to get the bone hierarchy
        node_stack.push_back(scene->mRootNode);
        while (!node_stack.empty()) {
            aiNode* current_node = node_stack[0];
            node_stack.erase(node_stack.begin());

            // Add children to the stack
            for (uint32_t node_child_index = 0; node_child_index < current_node->mNumChildren; node_child_index++) {
                node_stack.push_back(current_node->mChildren[node_child_index]);
            }

            // Check to see if this node is even a bone
            auto bone_id_it = model->bone_id_lookup.find(std::string(current_node->mName.C_Str()));
            if (bone_id_it == model->bone_id_lookup.end()) {
                continue;
            }

            int bone_id = bone_id_it->second;
            model->bones[bone_id].transform = assimp_mat4_to_siren_mat4(current_node->mTransformation);
            // Now iterate through each of the node's children...
            for (uint32_t child_index = 0; child_index < current_node->mNumChildren; child_index++) {
                // If the node child is not a bone, skip it
                auto bone_child_id_it = model->bone_id_lookup.find(std::string(current_node->mChildren[child_index]->mName.C_Str()));
                if (bone_child_id_it == model->bone_id_lookup.end()) {
                    continue;
                }

                // If it is a bone, add its index to the child_ids array
                int bone_child_id = bone_child_id_it->second;
                model->bones[bone_child_id].parent_id = bone_id;
            }

            SIREN_TRACE("Bone %i %s parent: %i", bone_id, current_node->mName.C_Str(), model->bones[bone_id].parent_id);
        } // End for each node in node stack

        // setup the mesh in OpenGL
        siren::Mesh mesh;
        mesh.index_count = indices.size();
        glGenVertexArrays(1, &mesh.vao);
        glGenBuffers(1, &mesh.vbo);
        glGenBuffers(1, &mesh.ebo);

        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

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

        // get material data
        aiMaterial* material = scene->mMaterials[meshes[i]->mMaterialIndex];

        // Material ambient color
        aiColor4D ambient_color(0.0f, 0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_AMBIENT, ambient_color);
        mesh.color_ambient = siren::vec3(ambient_color.r, ambient_color.g, ambient_color.b);

        // Material diffuse color
        aiColor4D diffuse_color(0.0f, 0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
        mesh.color_diffuse = siren::vec3(diffuse_color.r, diffuse_color.g, diffuse_color.b);

        // Material specular color
        aiColor4D specular_color(0.0f, 0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular_color);
        mesh.color_specular = siren::vec3(specular_color.r, specular_color.g, specular_color.b);

        // Material shininess
        material->Get(AI_MATKEY_SHININESS, mesh.shininess);
        material->Get(AI_MATKEY_SHININESS_STRENGTH, mesh.shininess_strength);

        if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0) {
            mesh.texture_diffuse = siren::texture_acquire("texture/empty.png");
        } else {
            aiString texture_path;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
            std::string full_texture_path = short_path_directory + std::string(texture_path.C_Str());
            mesh.texture_diffuse = siren::texture_acquire(full_texture_path.c_str());
        }

        if (material->GetTextureCount(aiTextureType_EMISSIVE) == 0) {
            mesh.texture_emissive = siren::texture_acquire("texture/empty_dark.png");
        } else {
            aiString texture_path;
            material->GetTexture(aiTextureType_EMISSIVE, 0, &texture_path);
            std::string full_texture_path = short_path_directory + std::string(texture_path.C_Str());
            mesh.texture_emissive = siren::texture_acquire(full_texture_path.c_str());
        }

        model->mesh.push_back(mesh);
    } // end for each mesh 

    // Get model animations
    SIREN_INFO("Model anims %u", scene->mNumAnimations);
    for (uint32_t a = 0; a < scene->mNumAnimations; a++) {
        SIREN_INFO("anim name %s duration %f ticks per second %f", scene->mAnimations[a]->mName.C_Str(), scene->mAnimations[a]->mDuration, scene->mAnimations[a]->mTicksPerSecond);

        // Create the animation in the list
        std::string animation_name = std::string(scene->mAnimations[a]->mName.C_Str());
        model->animations.push_back((siren::Animation) {
            .duration = (float)scene->mAnimations[a]->mDuration,
            .ticks_per_second = (float)scene->mAnimations[a]->mTicksPerSecond
        });
        for (int bone_id = 0; bone_id < model->bones.size(); bone_id++) {
            model->bones[bone_id].keyframes.push_back(siren::Keyframes());
        }
        uint32_t animation_id = model->bones[0].keyframes.size() - 1;
        model->animation_id_lookup[animation_name] = animation_id;

        // For each channel, add the respective bone's keyframes
        for (uint32_t n = 0; n < scene->mAnimations[a]->mNumChannels; n++) {
            aiNodeAnim* node_anim = scene->mAnimations[a]->mChannels[n];

            // Determine bone id of this channel
            auto bone_id_it = model->bone_id_lookup.find(std::string(node_anim->mNodeName.C_Str()));
            if (bone_id_it == model->bone_id_lookup.end()) {
                SIREN_TRACE("Animation %s does not correspond to a bone. Skipping...", node_anim->mNodeName.C_Str());
                continue;
            }
            int bone_id = bone_id_it->second;

            for (uint32_t position_index = 0; position_index < node_anim->mNumPositionKeys; position_index++) {
                aiVector3D position = node_anim->mPositionKeys[position_index].mValue;
                float t = node_anim->mPositionKeys[position_index].mTime; 
                model->bones[bone_id].keyframes[animation_id].positions.push_back((siren::KeyframeVec3) {
                    .value = siren::vec3(position.x, position.y, position.z),
                    .time = t
                });
            }

            for (uint32_t rotation_index = 0; rotation_index < node_anim->mNumRotationKeys; rotation_index++) {
                aiQuaternion rotation = node_anim->mRotationKeys[rotation_index].mValue;
                float t = node_anim->mRotationKeys[rotation_index].mTime; 
                model->bones[bone_id].keyframes[animation_id].rotations.push_back((siren::KeyframeQuat) {
                    .value = siren::quat(rotation.x, rotation.y, rotation.z, rotation.w),
                    .time = t
                });
            }

            for (uint32_t scale_index = 0; scale_index < node_anim->mNumScalingKeys; scale_index++) {
                aiVector3D scale = node_anim->mScalingKeys[scale_index].mValue;
                float t = node_anim->mScalingKeys[scale_index].mTime; 
                model->bones[bone_id].keyframes[animation_id].scales.push_back((siren::KeyframeVec3) {
                    .value = siren::vec3(scale.x, scale.y, scale.z),
                    .time = t
                });
            }
        } // End for each animation channel
    } // End for each animation

    glBindVertexArray(0);

    SIREN_TRACE("Model loaded successfully.");

    return true;
}
*/

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

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
            std::vector<std::vector<int>> bone_ids;
            std::vector<std::vector<float>> bone_weights;
            for (auto& attribute : primitive.attributes) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[attribute.second];
                const tinygltf::BufferView& buffer_view = gltf_model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = gltf_model.buffers[buffer_view.buffer];

                if (attribute.first.compare("POSITION") == 0) {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * sizeof(siren::vec3)); i += sizeof(siren::vec3)) {
                        siren::vec3 v;
                        std::memcpy(&v, &buffer.data.at(i), sizeof(siren::vec3));
                        positions.push_back(v);
                    }
                } else if (attribute.first.compare("NORMAL") == 0) {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * sizeof(siren::vec3)); i += sizeof(siren::vec3)) {
                        siren::vec3 v;
                        std::memcpy(&v, &buffer.data.at(i), sizeof(siren::vec3));
                        normals.push_back(v);
                    }
                } else if (attribute.first.compare("TEXCOORD_0") == 0) {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * sizeof(siren::vec2)); i += sizeof(siren::vec2)) {
                        siren::vec2 v;
                        std::memcpy(&v, &buffer.data.at(i), sizeof(siren::vec2));
                        tex_coords.push_back(v);
                    }
                } else if (attribute.first.compare("JOINTS_0") == 0) {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * sizeof(int[4])); i += sizeof(int[4])) {
                        std::vector<int> b(4, 0);
                        std::memcpy(&b[0], &buffer.data.at(i), 4 * sizeof(int));
                        bone_ids.push_back(b);
                    }
                } else if (attribute.first.compare("WEIGHTS_0") == 0) {
                    for (uint32_t i = buffer_view.byteOffset + accessor.byteOffset; i < buffer_view.byteOffset + accessor.byteOffset + (accessor.count * sizeof(float[4])); i += sizeof(float[4])) {
                        std::vector<float> b(4, 0.0f);
                        std::memcpy(&b[0], &buffer.data.at(i), 4 * sizeof(float));
                        bone_weights.push_back(b);
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
        for (uint32_t bone_index = 0; bone_index < skin.joints.size(); bone_index++) {
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
            SIREN_TRACE("transform:\n%m4\ninverse bind:\n%m4", model->bones[model->bones.size() - 1].transform, inverse_bind_transform);
        }
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
    model_transform->animation_timer = 0.0f;
}

void siren::model_transform_animation_update(ModelTransform* model_transform, float delta) {
    Model* model = model_transform->model;
    int animation_id = model_transform->animation;

    model_transform->animation_timer += model->animations[animation_id].ticks_per_second * delta;
    // Loops animation
    // TODO? Use assimp animation behavior?
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
            if (model_transform->animation_timer < bone_keyframes.positions[position_index].time) {
                float percent = (model_transform->animation_timer - bone_keyframes.positions[position_index].time) / (bone_keyframes.positions[position_index + 1].time - bone_keyframes.positions[position_index].time);
                bone_position = vec3::lerp(bone_keyframes.positions[position_index].value, bone_keyframes.positions[position_index + 1].value, percent);
                break;
            }
        }

        quat bone_rotation;
        for (uint32_t rotation_index = 0; rotation_index < bone_keyframes.rotations.size() - 1; rotation_index++) {
            if (model_transform->animation_timer < bone_keyframes.rotations[rotation_index].time) {
                float percent = (model_transform->animation_timer - bone_keyframes.rotations[rotation_index].time) / (bone_keyframes.rotations[rotation_index + 1].time - bone_keyframes.rotations[rotation_index].time);
                bone_rotation = quat::slerp(bone_keyframes.rotations[rotation_index].value, bone_keyframes.rotations[rotation_index + 1].value, percent);
                break;
            }
        }

        vec3 bone_scale;
        for (uint32_t scale_index = 0; scale_index < bone_keyframes.scales.size() - 1; scale_index++) {
            if (model_transform->animation_timer < bone_keyframes.scales[scale_index].time) {
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