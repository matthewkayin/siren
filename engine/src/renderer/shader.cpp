#include "shader.h"

#include "core/logger.h"
#include "core/resource.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define IO_READ_CHUNK_SIZE 262144

bool shader_compile(siren::Shader* id, GLenum shader_type, const char* p_path) {
    // determine full path
    char path[128];
    sprintf(path, "%s%s", siren::resource_get_base_path(), p_path);

    // open shader file
    FILE* shader_file = fopen(path, "rb");
    if (shader_file == NULL) {
        SIREN_ERROR("Unable to open shader at path %s", path);
        return false;
    }

    // TODO: move this out into a multipurpose file read function
    char* data = NULL;
    char* temp;
    size_t used = 0;
    size_t size = 0;
    size_t n;

    while (true) {
        if (used + IO_READ_CHUNK_SIZE + 1 > size) {
            size = used + IO_READ_CHUNK_SIZE + 1;

            // Overflow check
            if (size <= used) {
                free(data);
                SIREN_ERROR("Shader file %s too large\n", path);
                return false;
            }

            temp = (char*)realloc(data, size);
            if (temp == NULL) {
                SIREN_ERROR("Realloc failure while reading file %s", path);
                free(data);
                return false;
            }
            data = temp;
        }

        n = fread(data + used, 1, IO_READ_CHUNK_SIZE, shader_file);
        if (n == 0) {
            break;
        }

        used += n;
    }

    temp = (char*)realloc(data, used + 1);
    if (temp == NULL) {
        SIREN_ERROR("Realloc failure while reading file %s", path);
        free(data);
        return false;
    }
    data = temp;
    data[used] = '\0';

    // compile the shader
    int success;
    *id = glCreateShader(shader_type);
    glShaderSource(*id, 1, &data, NULL);
    glCompileShader(*id);
    glGetShaderiv(*id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(*id, 512, NULL, info_log);
        SIREN_ERROR("Shader %s failed to compile: %s", path, info_log);
        return false;
    }

    // cleanup
    fclose(shader_file);

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

void siren::shader_set_uniform(siren::Shader id, const char* name, int value) {
    glUniform1i(glGetUniformLocation(id, name), value);
}

void siren::shader_set_uniform(siren::Shader id, const char* name, uint32_t value) {
    glUniform1ui(glGetUniformLocation(id, name), value);
}

void siren::shader_set_uniform(siren::Shader id, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(id, name), (int)value);
}

void siren::shader_set_uniform(siren::Shader id, const char* name, siren::ivec2 value) {
    glUniform2iv(glGetUniformLocation(id, name), 1, value.elements);
}

void siren::shader_set_uniform(siren::Shader id, const char* name, siren::vec2 value) {
    glUniform2fv(glGetUniformLocation(id, name), 1, value.elements);
}

void siren::shader_set_uniform(siren::Shader id, const char* name, siren::vec3 value) {
    glUniform3fv(glGetUniformLocation(id, name), 1, value.elements);
}

void siren::shader_set_uniform(siren::Shader id, const char* name, siren::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, value.elements);
}