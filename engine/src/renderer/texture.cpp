#include "texture.h"

#include "core/logger.h"
#include "core/resource.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

siren::Texture siren::texture_load(const char* _path) {
    // determine full path
    char path[128];
    sprintf(path, "%s%s", siren::resource_get_base_path(), _path);

    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int number_of_components;
    stbi_uc* data = stbi_load(path, &width, &height, &number_of_components, 0);
    if (!data) {
        SIREN_LOG_ERROR("Could not load texture %s", path);
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
        SIREN_LOG_ERROR("Texture format of texture %s not recognized.", path);
    }

    uint32_t texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, width, height, GL_FALSE, texture_format, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}