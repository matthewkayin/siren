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
static std::unordered_map<siren::Texture, std::vector<siren::TextureArrayInfo>> texture_array_info;

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

static std::unordered_map<uint32_t, siren::Texture> solidcolor_textures;

siren::Texture siren::texture_acquire_solidcolor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    uint32_t color = (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);

    // Check if a solid color texture of this color has already been created
    auto it = solidcolor_textures.find(color);
    if (it != solidcolor_textures.end()) {
        return it->second;
    }

    // Otherwise create a new one
    uint32_t texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, &color);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    solidcolor_textures[color] = texture;
    return texture;
}

#include <string>
#include <fstream>
#include <unordered_map>
#include <math.h>
siren::Texture siren::texture_array_create(std::string name, const std::vector<std::string>& texture_paths) {
    // Load all the image data first so that we can get the max width and height of a texture
    std::vector<stbi_uc*> texture_data;
    std::vector<TextureArrayInfo> texture_info;
    int max_texture_width = 0;
    int max_texture_height = 0;
    for (uint32_t i = 0; i < texture_paths.size(); i++) {
        std::string full_path = resource_get_base_path() + texture_paths[i];
        SIREN_TRACE("Loading texture %s...", full_path.c_str());

        int texture_width;
        int texture_height;
        int texture_component_count;

        stbi_uc* data = stbi_load(full_path.c_str(), &texture_width, &texture_height, &texture_component_count, 0);
        if (!data) {
            SIREN_ERROR("Could not load texture %s.", full_path.c_str());
            return 0;
        }
        max_texture_width = std::max(texture_width, max_texture_width);
        max_texture_height = std::max(texture_height, max_texture_height);

        texture_data.push_back(data);
        texture_info.push_back((TextureArrayInfo) {
            .size = ivec2(texture_width, texture_height)
        });
    }

    // Create the texture array
    siren::Texture texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, max_texture_width, max_texture_height, texture_paths.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    for (uint32_t i = 0; i < texture_data.size(); i++) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, texture_info[i].size.x, texture_info[i].size.y, 1, GL_RGBA, GL_UNSIGNED_BYTE, texture_data[i]);
        stbi_image_free(texture_data[i]);
    }

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    textures[name] = texture;
    texture_array_info[texture] = texture_info;

    SIREN_INFO("Texture array loaded successfully.");
    return texture;
}

bool siren::texture_is_texture_array(siren::Texture texture) {
    return texture_array_info.find(texture) != texture_array_info.end();
}

const std::vector<siren::TextureArrayInfo>& siren::texture_array_info_get(siren::Texture texture) {
    return texture_array_info[texture];
}