#include "shader.h"

#include "core/logger.h"
#include "core/resource.h"

#include <glad/glad.h>

#include <fstream>
#include <string>

bool shader_compile(siren::Shader* id, GLenum shader_type, const char* path) {
    // determine full path
    std::string full_path = siren::resource_get_base_path() + std::string(path);

    // read the shader file
    std::string shader_source;
    std::ifstream shader_file;
    std::string line;

    shader_file.open(full_path);
    if (!shader_file.is_open()) {
        SIREN_ERROR("Error opening shader file at path %s", full_path.c_str());
        return false;
    }

    while (std::getline(shader_file, line)) {
        shader_source += line + "\n";
    }

    shader_file.close();
    
    // compile the shader
    const char* shader_source_cstr = shader_source.c_str();
    int success;
    *id = glCreateShader(shader_type);
    glShaderSource(*id, 1, &shader_source_cstr, NULL);
    glCompileShader(*id);
    glGetShaderiv(*id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(*id, 512, NULL, info_log);
        SIREN_ERROR("Shader %s failed to compile: %s", path, info_log);
        return false;
    }

    return true;
}

bool siren::shader_load(siren::Shader* id, const char* vertex_path, const char* fragment_path) {
    // Compile shaders
    GLuint vertex_shader;
    if (!shader_compile(&vertex_shader, GL_VERTEX_SHADER, vertex_path)) {
        return false;
    }

    GLuint fragment_shader;
    if (!shader_compile(&fragment_shader, GL_FRAGMENT_SHADER, fragment_path)) {
        return false;
    }

    // Link program
    int success;
    *id = glCreateProgram();
    glAttachShader(*id, vertex_shader);
    glAttachShader(*id, fragment_shader);
    glLinkProgram(*id);
    glGetProgramiv(*id, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(*id, 512, NULL, info_log);
        SIREN_ERROR("Failed linking shader program. Vertex: %s Fragment %s Error: %s", vertex_path, fragment_path, info_log);
        return false;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return true;
}

void siren::shader_use(siren::Shader id) {
    glUseProgram(id);
}

// TODO: only call getUniformLocation once somehow?

void siren::shader_set_uniform_int(siren::Shader id, const char* name, int value) {
    glUniform1i(glGetUniformLocation(id, name), value);
}

void siren::shader_set_uniform_uint(siren::Shader id, const char* name, uint32_t value) {
    glUniform1ui(glGetUniformLocation(id, name), value);
}

void siren::shader_set_uniform_bool(siren::Shader id, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(id, name), (int)value);
}

void siren::shader_set_uniform_float(siren::Shader id, const char* name, float value) {
    glUniform1f(glGetUniformLocation(id, name), value);
}

void siren::shader_set_uniform_ivec2(siren::Shader id, const char* name, siren::ivec2 value) {
    glUniform2iv(glGetUniformLocation(id, name), 1, value.elements);
}

void siren::shader_set_uniform_vec2(siren::Shader id, const char* name, siren::vec2 value) {
    glUniform2fv(glGetUniformLocation(id, name), 1, value.elements);
}

void siren::shader_set_uniform_vec3(siren::Shader id, const char* name, siren::vec3 value) {
    glUniform3fv(glGetUniformLocation(id, name), 1, value.elements);
}

void siren::shader_set_uniform_vec4(siren::Shader id, const char* name, siren::vec4 value) {
    glUniform4fv(glGetUniformLocation(id, name), 1, value.elements);
}

void siren::shader_set_uniform_mat4(siren::Shader id, const char* name, siren::mat4* value, uint32_t size) {
    glUniformMatrix4fv(glGetUniformLocation(id, name), size, GL_FALSE, (float*)value);
}