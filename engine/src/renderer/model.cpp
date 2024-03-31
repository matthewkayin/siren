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
            // Note: assimp models might have multiple texture coordinates. currently we are naively assuming that we can just use coordinate set 0,
            // but later on we might need to write some code which decides which coordinates to use`
            vertices.push_back((VertexData) {
                .position = siren::vec3(meshes[i]->mVertices[v].x, meshes[i]->mVertices[v].y, meshes[i]->mVertices[v].z),
                .normal = siren::vec3(meshes[i]->mNormals[v].x, meshes[i]->mNormals[v].y, meshes[i]->mNormals[v].z),
                .texture_coordinate = siren::vec2(meshes[i]->mTextureCoords[0][v].x, meshes[i]->mTextureCoords[0][v].y),
                .bone_ids = { -1, -1, -1, -1 },
                .bone_weights = { 0.0f, 0.0f, 0.0f, 0.0f }
            });
        }

        // determine the mesh offset, which is the center of its coordinates in each direction
        siren::vec3 position_max = vertices[0].position;
        siren::vec3 position_min = position_max;
        for (uint32_t v = 1; v < vertices.size(); v++) {
            position_max.x = siren::max(position_max.x, vertices[i].position.x);
            position_max.y = siren::max(position_max.x, vertices[i].position.y);
            position_max.z = siren::max(position_max.x, vertices[i].position.z);

            position_min.x = siren::min(position_min.x, vertices[i].position.x);
            position_min.y = siren::min(position_min.x, vertices[i].position.y);
            position_min.z = siren::min(position_min.x, vertices[i].position.z);
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

            std::string bone_name = std::string(meshes[i]->mBones[assimp_bone_index]->mName.C_Str());
            auto bone_id_it = model->bone_id_lookup.find(bone_name);
            if (bone_id_it == model->bone_id_lookup.end()) {
                siren::Bone new_bone = (siren::Bone) {
                    .child_ids = { -1, -1, -1, -1 },
                    .keyframes = std::vector<std::vector<siren::Transform>>(),
                    .offset = assimp_mat4_to_siren_mat4(meshes[i]->mBones[assimp_bone_index]->mOffsetMatrix)
                };
                SIREN_TRACE("New bone with name %s id %i", bone_name.c_str(), model->bones.size());
                model->bones.push_back(new_bone);
                bone_id = model->bones.size() - 1;
                model->bone_id_lookup[bone_name] = bone_id;
            } else {
                bone_id = bone_id_it->second;
            }

            SIREN_ASSERT(bone_id != -1);

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
                }
            }
        }

        // Traverse the node stack to get the bone hierarchy
        node_stack.push_back(scene->mRootNode);

        while (!node_stack.empty()) {
            aiNode* current_node = node_stack[node_stack.size() - 1];
            node_stack.pop_back();

            // Add children to the stack
            for (uint32_t node_child_index = 0; node_child_index < current_node->mNumChildren; node_child_index++) {
                node_stack.push_back(current_node->mChildren[node_child_index]);
            }

            // Check to see if this node is even a bone
            auto bone_id_it = model->bone_id_lookup.find(std::string(current_node->mName.C_Str()));
            if (bone_id_it == model->bone_id_lookup.end()) {
                continue;
            }

            // Make sure that the bone child_ids array is big enough for the number of children on this node
            SIREN_ASSERT(current_node->mNumChildren < 5);
            int bone_id = bone_id_it->second;
            uint32_t bone_child_count = 0;
            // Now iterate through each of the node's children...
            for (uint32_t child_index = 0; child_index < current_node->mNumChildren; child_index++) {
                // If the node child is not a bone, skip it
                auto bone_child_id_it = model->bone_id_lookup.find(std::string(current_node->mChildren[child_index]->mName.C_Str()));
                if (bone_child_id_it == model->bone_id_lookup.end()) {
                    continue;
                }

                // If it is a bone, add its index to the child_ids array
                int bone_child_id = bone_child_id_it->second;
                model->bones[bone_id].child_ids[bone_child_count] = bone_child_id;
                bone_child_count++;
            }

            SIREN_TRACE("Bone %i children: %i %i %i %i", bone_id, model->bones[bone_id].child_ids[0], model->bones[bone_id].child_ids[1], model->bones[bone_id].child_ids[2], model->bones[bone_id].child_ids[3]);
        }

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

    if (!model_animation_add(model, "open", "../res/model/door/anims/open.smd")) {
        return false;
    }

    glBindVertexArray(0);

    SIREN_TRACE("Model loaded successfully.");

    return true;
}

bool siren::model_animation_add(siren::Model* model, const char* name, const char* path) {
    enum ParseMode {
        PARSE_MODE_NONE,
        PARSE_MODE_NODES,
        PARSE_MODE_SKELETON
    };
    ParseMode parse_mode = PARSE_MODE_NONE;
    uint32_t animation_frame = 0;

    std::ifstream animation_file;
    std::string line;

    std::unordered_map<int, int> node_to_bone_map;

    animation_file.open(path);
    if (!animation_file.is_open()) {
        SIREN_ERROR("Unable to open animation file at path %s.", path);
        return false;
    }

    // Create a new set of keyframes for this animation
    int animation_id = model->bones[0].keyframes.size();
    for (uint32_t bone_index = 0; bone_index < model->bones.size(); bone_index++) {
        model->bones[bone_index].keyframes.push_back(std::vector<Transform>());
    }
    model->animation_id_lookup[std::string(name)] = animation_id;

    while (std::getline(animation_file, line)) {
        if (line[0] == '/' && line[1] == '/') {
            continue;
        } else if (line == "end") {
            parse_mode = PARSE_MODE_NONE;
        } else if (line == "nodes") {
            parse_mode = PARSE_MODE_NODES;
        } else if (line == "skeleton") {
            parse_mode = PARSE_MODE_SKELETON;
        } else {
            if (parse_mode == PARSE_MODE_NONE) {
                continue;
            }

            // Remove leading whitespace
            size_t non_space_index = line.find_first_not_of(' ');
            line = line.substr(non_space_index);

            // Break line into words
            std::vector<std::string> words;
            while (line.size() != 0) {
                size_t space_index = line.find_first_of(' ');
                if (space_index == std::string::npos) {
                    words.push_back(line);
                    line = "";
                } else {
                    words.push_back(line.substr(0, space_index));
                    line = line.substr(space_index + 1);
                }
            }

            // Parse the words
            if (parse_mode == PARSE_MODE_NODES) {
                // node entry takes the form:
                // <node-id> "<node-name>" <node-root>
                // here we ignore any nodes that aren't bones
                // and we create a mapping from the node_id in the anim file to the bone_id in the model data
                int node_id = std::stoi(words[0]);
                std::string animation_bone_name = words[1].substr(1, words[1].size() - 2); // This substr removes the quotes from the string

                auto model_bone_id_it = model->bone_id_lookup.find(animation_bone_name);
                if (model_bone_id_it == model->bone_id_lookup.end()) {
                    continue;
                }
                int model_bone_id = model_bone_id_it->second;
                
                node_to_bone_map[node_id] = model_bone_id;
            } else if (parse_mode == PARSE_MODE_SKELETON) {
                if (words[0] == "time") {
                    // time entry takes the form:
                    // time <frame-number>
                    animation_frame = std::stoi(words[1]);

                    // for each frame we add a new entry to the keyframes of all the bones
                    // the first frame in an animation will always have an entry for each bone's initial transform
                    // subsequency frames are allowed to omit a transform for a bone, which means the bone stays in the same place
                    // to account for this, keyframes will be initialized using the previous frame 
                    for (uint32_t bone_index = 0; bone_index < model->bones.size(); bone_index++) {
                        model->bones[bone_index].keyframes[animation_id].push_back(animation_frame == 0 ? (Transform) {
                            .position = vec3(),
                            .rotation = quat(),
                            .scale = vec3(1.0f)
                        } : model->bones[bone_index].keyframes[animation_id][animation_frame - 1]);
                    }
                } else {
                    // Get the bone_id
                    int animation_bone_id = std::stoi(words[0]);
                    auto bone_id_it = node_to_bone_map.find(animation_bone_id);
                    if (bone_id_it == node_to_bone_map.end()) {
                        continue;
                    }
                    int bone_id = bone_id_it->second;

                    // Add the keyframe
                    model->bones[bone_id].keyframes[animation_id][animation_frame] = (Transform) {
                        .position = vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3])),
                        .rotation = 
                        quat::from_axis_angle(VEC3_FORWARD, std::stof(words[6]), true) *
                        quat::from_axis_angle(VEC3_UP, std::stof(words[5]), true) * 
                        quat::from_axis_angle(VEC3_RIGHT, std::stof(words[4]), true),
                        .scale = vec3(1.0f)
                    };
                }
            }
        }
    }

    animation_file.close();

    return true;
}

uint32_t siren::model_animation_get_frame_count(Model* model, int animation_id) {
    return model->bones[0].keyframes[animation_id].size();
}

siren::ModelTransform siren::model_transform_create(siren::Model* model) {
    ModelTransform result;

    result.model = model;
    result.root_transform = transform_identity();
    result.root_transform.position.z = -5.0f;
    result.root_transform.scale = vec3(0.05f);

    std::vector<Transform> bone_transform;
    for (uint32_t bone_index = 0; bone_index < model->bones.size(); bone_index++) {
        bone_transform.push_back(transform_identity());
    }
    result.bone_transform = bone_transform;

    result.animation = ModelTransform::ANIMATION_NONE;
    result.animation_frame = 0;
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
    model_transform->animation_frame = 0;
    model_transform->animation_timer = 0.0f;
}

void siren::model_transform_animation_update(ModelTransform* model_transform, float delta) {
    static const float FRAME_DURATION = 0.25f;

    model_transform->animation_timer += delta;
    if (model_transform->animation_timer > FRAME_DURATION) {
        model_transform->animation_timer -= FRAME_DURATION;

        model_transform->animation_frame++;
        if (model_transform->animation_frame == model_animation_get_frame_count(model_transform->model, model_transform->animation)) {
            model_transform->animation_frame = 0;
        }
    }

    for (uint32_t bone_index = 0; bone_index < model_transform->bone_transform.size(); bone_index++) {
        model_transform->bone_transform[bone_index] = model_transform->model->bones[bone_index].keyframes[model_transform->animation][model_transform->animation_frame];
    }
}