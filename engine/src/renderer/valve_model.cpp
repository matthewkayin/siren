#include "model.h"

#include "core/logger.h"
#include "core/asserts.h"
#include "core/resource.h"

#include <glad/glad.h>

#include <fstream>

static std::unordered_map<std::string, siren::Model> models;

bool valve_model_load(siren::Model* model, siren::ValveModelAcquireParams params);

siren::Model* siren::valve_model_acquire(siren::ValveModelAcquireParams params) {
    std::string key = std::string(params.qc_path);
    auto it = models.find(key);
    if (it != models.end()) {
        return &it->second;
    }

    Model model;
    if (!valve_model_load(&model, params)) {
        return nullptr;
    }

    models[key] = model;
    return &models[key];
}

void trim_and_split_line(const std::string line, std::vector<std::string>* words) {
    // Remove leading whitespace
    size_t non_space_index = line.find_first_not_of(' ');
    std::string trimmed_line = line.substr(non_space_index);

    // Break line into words
    while (trimmed_line.size() != 0) {
        size_t space_index = trimmed_line.find_first_of(' ');
        if (space_index == std::string::npos) {
            words->push_back(trimmed_line);
            trimmed_line = "";
        } else {
            words->push_back(trimmed_line.substr(0, space_index));
            trimmed_line = trimmed_line.substr(space_index + 1);
        }
    }
}

bool valve_model_load(siren::Model* model, siren::ValveModelAcquireParams params) {
    enum SmdParseMode {
        SMD_PARSE_MODE_NONE,
        SMD_PARSE_MODE_NODE,
        SMD_PARSE_MODE_SKELETON,
        SMD_PARSE_MODE_TRIANGLES
    };

    struct VertexData {
        siren::vec3 position;
        siren::vec3 normal;
        siren::vec2 texture_coordinate;
        int bone_ids[SIREN_MAX_BONE_INFLUENCE];
        float bone_weights[SIREN_MAX_BONE_INFLUENCE];
    };

    struct MeshData {
        std::vector<VertexData> vertices;
        std::unordered_map<std::string, int> vertex_id_lookup;
        std::vector<uint32_t> indices;
    };

    std::ifstream smd_file;
    std::string smd_full_path = siren::resource_get_base_path() + params.smd_path;
    SIREN_TRACE("Loading model %s...", smd_full_path.c_str());
    smd_file.open(smd_full_path);
    if (!smd_file.is_open()) {
        SIREN_ERROR("Unable to open file %s", smd_full_path.c_str());
        return false;
    }

    std::unordered_map<std::string, MeshData> mesh_data;

    SmdParseMode smd_parse_mode = SMD_PARSE_MODE_NONE;
    std::string line;
    while (std::getline(smd_file, line)) {
        if (line[0] == '/' && line[1] == '/') {
            continue;
        } else if (line == "end") {
            smd_parse_mode = SMD_PARSE_MODE_NONE;
        } else if (line == "triangles") {
            smd_parse_mode = SMD_PARSE_MODE_TRIANGLES;
        } else if (line == "nodes") {
            smd_parse_mode = SMD_PARSE_MODE_NODE;
        } else if (line == "skeleton") {
            smd_parse_mode = SMD_PARSE_MODE_SKELETON;
        } else if (smd_parse_mode == SMD_PARSE_MODE_NONE) {
            continue;
        } else if (smd_parse_mode == SMD_PARSE_MODE_NODE) {
            std::vector<std::string> words;
            trim_and_split_line(line, &words);

            // Each bone line contains the form <bone_id> <bone_name> <parent_bone_id>
            // Bones are generally listed in order in the file, this check just confirms that that is true
            int bone_id = model->bones.size();
            SIREN_ASSERT(bone_id == std::stoi(words[0]));

            model->bones.push_back((siren::Bone) {
                .parent_id = std::stoi(words[2]),
                .keyframes = std::vector<std::vector<siren::Transform>>(),
                .initial_transform = siren::basis_transform_identity()
            });
        } else if (smd_parse_mode == SMD_PARSE_MODE_SKELETON) {
            std::vector<std::string> words;
            trim_and_split_line(line, &words);

            if (words[0] == "time") {
                // In a model file, the skeleton block will define initial positions for each bone
                for (int bone_id = 0; bone_id < model->bones.size(); bone_id++) {
                    words.clear();
                    std::getline(smd_file, line);
                    trim_and_split_line(line, &words);

                    SIREN_ASSERT(bone_id == std::stoi(words[0]));

                    model->bones[bone_id].initial_transform = siren::basis_transform_create(siren::vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3])), siren::vec3(std::stof(words[4]), std::stof(words[5]), std::stof(words[6])));
                    model->bones[bone_id].inverse_bind_transform = siren::basis_transform_to_matrix(model->bones[bone_id].initial_transform).inversed();
                    SIREN_TRACE("initial x inverse \n%m4", siren::basis_transform_to_matrix(model->bones[bone_id].initial_transform) * model->bones[bone_id].inverse_bind_transform);
                    /* model->bones[bone_id].initial_transform = (siren::Transform) {
                        .position = siren::vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3])),
                        .rotation = 
                            siren::quat::from_axis_angle(siren::VEC3_RIGHT, std::stof(words[4]), true) *
                            siren::quat::from_axis_angle(siren::VEC3_UP, std::stof(words[5]), true) *
                            siren::quat::from_axis_angle(siren::VEC3_FORWARD, std::stof(words[6]), true),
                        .scale = siren::vec3(1.0f)
                    }; */
                }
            }
        } else if (smd_parse_mode == SMD_PARSE_MODE_TRIANGLES) {
            // Determine the name of the mesh
            auto it = mesh_data.find(line);
            if (it == mesh_data.end()) {
                mesh_data[line] = (MeshData) {
                    .vertices = std::vector<VertexData>(),
                    .vertex_id_lookup = std::unordered_map<std::string, int>(),
                    .indices = std::vector<uint32_t>()
                };
                it = mesh_data.find(line);
            }

            // The next three lines will now be the three vertices of a triangle
            for (uint32_t triangle_index = 0; triangle_index < 3; triangle_index++) {
                // Read the line and split into words
                std::getline(smd_file, line);
                std::vector<std::string> words;
                trim_and_split_line(line, &words);

                // Check to see if we already have this vertex in the vertex data array
                auto vertex_id_it = it->second.vertex_id_lookup.find(line);
                if (vertex_id_it != it->second.vertex_id_lookup.end()) {
                    it->second.indices.push_back(vertex_id_it->second);
                    continue;
                }

                // Get the vertex data from the line
                VertexData vertex = (VertexData) {
                    .position = siren::vec3(std::stof(words[1]), std::stof(words[2]), std::stof(words[3])),
                    .normal = siren::vec3(std::stof(words[4]), std::stof(words[5]), std::stof(words[6])),
                    .texture_coordinate = siren::vec2(std::stof(words[7]), std::stof(words[8])),
                    .bone_ids = { -1, -1, -1, -1 },
                    .bone_weights = { 0.0f, 0.0f, 0.0f, 0.0f }
                };

                // Get the bone weights from the line
                int parent_bone = std::stoi(words[0]);
                float parent_bone_weight = 1.0f;

                // Position 9 is optional, if it exists, then it will define a series of bone ids and weights
                if (words.size() > 9) {
                    int links = std::stoi(words[9]);
                    SIREN_ASSERT(links <= SIREN_MAX_BONE_INFLUENCE);
                    for (int link_index = 0; link_index < links; link_index++) {
                        vertex.bone_ids[link_index] = std::stoi(words[10 + (link_index * 2)]);
                        vertex.bone_weights[link_index] = std::stoi(words[11 + (link_index * 2)]);
                        parent_bone_weight -= vertex.bone_weights[link_index];
                    }

                    // If the series of bone weights does not add up to 1, then the remaining weight is added to the parent bone
                    if (parent_bone_weight != 0.0f) {
                        SIREN_ASSERT(links < SIREN_MAX_BONE_INFLUENCE);
                        vertex.bone_ids[links] = parent_bone;
                        vertex.bone_weights[links] = parent_bone_weight;
                    }
                // If there is no series of bone ids / weights, use the parent bone
                } else {
                    vertex.bone_ids[0] = parent_bone;
                    vertex.bone_weights[0] = parent_bone_weight;
                }

                // Add the vertex to the mesh
                it->second.vertices.push_back(vertex);
                it->second.vertex_id_lookup[line] = it->second.vertices.size() - 1;
                it->second.indices.push_back(it->second.vertices.size() - 1);
            } // End for each triangle index
        } // End if parse mode == SMD_PARSE_MODE_TRIANGLES
    } // End while getline()

    smd_file.close();

    // Generate meshes
    for (auto it = mesh_data.begin(); it != mesh_data.end(); it++) {
        // Create a VAO for each mesh
        siren::Mesh mesh;
        mesh.index_count = it->second.indices.size();

        glGenVertexArrays(1, &mesh.vao);
        glGenBuffers(1, &mesh.vbo);
        glGenBuffers(1, &mesh.ebo);

        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, it->second.vertices.size() * sizeof(VertexData), &it->second.vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, it->second.indices.size() * sizeof(uint32_t), &it->second.indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texture_coordinate));
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, SIREN_MAX_BONE_INFLUENCE, GL_INT, sizeof(VertexData), (void*)offsetof(VertexData, bone_ids));
        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(4, SIREN_MAX_BONE_INFLUENCE, GL_INT, sizeof(VertexData), (void*)offsetof(VertexData, bone_weights));

        // Load the texture for each mesh
        mesh.color_ambient = siren::vec3(0.2f);
        mesh.color_diffuse = siren::vec3(0.5f);
        mesh.color_specular = siren::vec3(1.0f);
        mesh.shininess = 32.0f;
        mesh.shininess_strength = 1.0f;
        mesh.texture_diffuse = siren::texture_acquire(params.diffuse_path);
        mesh.texture_emissive = siren::texture_acquire("texture/empty_dark.png");

        model->mesh.push_back(mesh);
    }

    glBindVertexArray(0);

    SIREN_TRACE("Valve model loaded successfully.");
    return true;
}