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

bool model_load(siren::Model* model, const char* path);

siren::Model* siren::model_acquire(const char* path) {
    uint32_t index = models.get_index_of_key(path);
    if (index != siren::Hashtable<siren::Model>::ENTRY_NOT_FOUND) {
        return &models.get_data_at_index(index);
    }

    Model model;
    if (!model_load(&model, path)) {
        return nullptr;
    }

    index = models.insert(path, model);
    return &models.get_data_at_index(index);
}

bool model_load(siren::Model* model, const char* path) {
    std::string short_path = std::string(path);
    std::string full_path = siren::resource_get_base_path() + short_path;
    std::string short_path_directory = short_path.substr(0, short_path.find_last_of("/") + 1);

    SIREN_TRACE("Loading model %s...", full_path.c_str());
    SIREN_TRACE("Short path directory is: %s", short_path_directory.c_str());

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
                .texture_coordinate = siren::vec2(meshes[i]->mTextureCoords[0][v].x, meshes[i]->mTextureCoords[0][v].y)
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

        SIREN_TRACE("Vertex count: %u | Index count: %u | Mesh offset: %v3", vertices.size(), indices.size(), mesh_offset);

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

        // get material data
        aiMaterial* material = scene->mMaterials[meshes[i]->mMaterialIndex];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0) {
            mesh.map_diffuse = siren::texture_acquire("texture/empty.png");
        } else {
            aiString texture_path;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
            SIREN_TRACE("Texture path is %s", texture_path.C_Str());
            std::string full_texture_path = short_path_directory + std::string(texture_path.C_Str());
            SIREN_TRACE("Full texture path is %s", full_texture_path.c_str());
            mesh.map_diffuse = siren::texture_acquire(full_texture_path.c_str());
        }

        model->mesh.push(mesh);
    } // end for each mesh 

    glBindVertexArray(0);

    SIREN_TRACE("Loaded model %s", path);

    return true;
}