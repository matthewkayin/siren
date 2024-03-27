#include "model.h"

#include "core/logger.h"
#include "core/resource.h"

#include "containers/hashtable.h"

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

static siren::Hashtable<siren::Model> models(32);

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
    uint32_t index = models.get_index(path);
    if (index != SIREN_HASHTABLE_ENTRY_NOT_FOUND) {
        return &models.get_data(index);
    }

    Model model;
    if (!model_load(&model, path)) {
        return nullptr;
    }

    index = models.insert(path, model);
    return &models.get_data(index);
}

bool model_load(siren::Model* model, const char* path) {
    std::string short_path = std::string(path);
    std::string full_path = siren::resource_get_base_path() + short_path;
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
        SIREN_TRACE("Creating mesh index %u...", i);
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
        siren::vec3 mesh_offset = position_min + ((position_max - position_min) * 0.5f);

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

            const char* bone_name = meshes[i]->mBones[assimp_bone_index]->mName.C_Str();
            uint32_t bone_hash_index = model->bones.get_index(bone_name);
            if (bone_hash_index == SIREN_HASHTABLE_ENTRY_NOT_FOUND) {
                siren::Bone new_bone = (siren::Bone) {
                    .id = (int)model->bones.size(),
                    .offset = assimp_mat4_to_siren_mat4(meshes[i]->mBones[assimp_bone_index]->mOffsetMatrix)
                };
                SIREN_TRACE("New bone with name %s id %i offset \n%m4", bone_name, new_bone.id, new_bone.offset);
                bone_hash_index = model->bones.insert(bone_name, new_bone);
            } 
            bone_id = model->bones.get_data(bone_hash_index).id;

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
        
        // setup the mesh in OpenGL
        siren::Mesh mesh;
        mesh.index_count = indices.size();
        mesh.offset = siren::vec3(0.0f);
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

        SIREN_TRACE("bones? %u anims? %u", meshes[i]->mNumBones, scene->mNumAnimations);

        model->mesh.push(mesh);
    } // end for each mesh 

    glBindVertexArray(0);

    SIREN_TRACE("Model loaded successfully.");

    return true;
}