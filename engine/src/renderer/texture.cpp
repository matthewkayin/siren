#include "texture.h"

#include "core/logger.h"
#include "core/resource.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <unordered_map>

static std::unordered_map<const char*, siren::Texture> textures;

siren::Texture siren::texture_system_acquire(const char* path) {
    SIREN_LOG_DEBUG("Requested texture %s", path);
    // check if texture has been loaded
    std::unordered_map<const char*, Texture>::iterator it = textures.find(path);
    if (it != textures.end()) {
        SIREN_LOG_DEBUG("Texture already loaded, returning copy.");
        return it->second;
    }

    // determine full path
    std::string full_path = resource_get_base_path() + std::string(path);
    SIREN_LOG_DEBUG("Loading texture %s...", full_path.c_str());

    stbi_set_flip_vertically_on_load(false);
    int width;
    int height;
    int number_of_components;
    stbi_uc* data = stbi_load(full_path.c_str(), &width, &height, &number_of_components, 0);
    if (!data) {
        SIREN_LOG_ERROR("Could not load texture %s", full_path.c_str());
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
        SIREN_LOG_ERROR("Texture format of texture %s not recognized.", full_path.c_str());
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

    textures[path] = texture;
    SIREN_LOG_DEBUG("Texture loaded successfully.");
    return texture;
}