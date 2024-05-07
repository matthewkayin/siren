#include "texture.h"

#define SIREN_VTF_MAX_SUPPORTED_RESOURCES 32

#include "core/logger.h"
#include "core/resource.h"
#include "core/asserts.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <unordered_map>
#include <cstdio>

static std::unordered_map<std::string, siren::Texture> textures;

siren::Texture texture_load(const char* path);

siren::Texture siren::texture_acquire(const char* path) {
    std::string key = std::string(path);

    // determine full path
    std::string full_path = resource_get_base_path() + key;
    SIREN_TRACE("Loading texture %s...", full_path.c_str());

    // check if texture has been loaded
    auto it = textures.find(key);
    if (it != textures.end()) {
        SIREN_TRACE("Texture already loaded, returning copy.");
        return it->second;
    }

    Texture texture = texture_load(full_path.c_str());

    if (texture != 0) {
        textures[key] = texture;
        SIREN_TRACE("Texture loaded successfully.");
    }

    return texture;
}

siren::Texture texture_load(const char* path) {
    // TODO, call this only once?
    stbi_set_flip_vertically_on_load(false);
    int width;
    int height;
    int number_of_components;
    stbi_uc* data = stbi_load(path, &width, &height, &number_of_components, 0);
    if (!data) {
        SIREN_ERROR("Could not load texture %s", path);
        return 0;
    }

    GLenum texture_format;
    if (number_of_components == 1) {
        texture_format = GL_RED;
    } else if (number_of_components == 3) {
        texture_format = GL_RGB;
    } else if (number_of_components == 4) {
        texture_format = GL_RGBA;
    } else {
        SIREN_ERROR("Texture format of texture %s not recognized.", path);
        return 0;
    }

    uint32_t texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, width, height, GL_FALSE, texture_format, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}