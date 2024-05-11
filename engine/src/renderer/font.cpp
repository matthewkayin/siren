#include "font.h"

#include "core/logger.h"
#include "core/resource.h"
#include "math/math.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstring>
#include <cstdio>
#include <unordered_map>

static std::vector<siren::Font> fonts;
static std::unordered_map<std::string, siren::FontHandle> font_handles;

bool font_load(siren::Font* font, std::string path, uint16_t size);

siren::FontHandle siren::font_acquire(const char* path, uint16_t size) {
    std::string key = std::string(path) + std::string(":") + std::to_string(size);

    // Check if font has been loaded
    auto it = font_handles.find(key);
    if (it != font_handles.end()) {
        return it->second;
    }

    // Begin creating a new font
    Font font;
    std::string full_path = resource_get_base_path() + std::string(path);
    if (!font_load(&font, full_path, size)) {
        return FONT_HANDLE_NULL;
    }

    fonts.push_back(font);
    FontHandle handle = fonts.size() - 1;
    font_handles[key] = handle;
    return handle;
}

const siren::Font& siren::font_get(siren::FontHandle handle) {
    return fonts[handle];
}

bool font_load(siren::Font* font, std::string path, uint16_t size) {
    static const SDL_Color COLOR_WHITE = { 255, 255, 255, 255 };

    TTF_Font* ttf_font = TTF_OpenFont(path.c_str(), size);
    if (ttf_font == NULL) {
        SIREN_ERROR("Unable to open font at path %s. SDL Error: %s", path.c_str(), TTF_GetError());
        return false;
    }

    // Render each glyph to a surface
    SDL_Surface* glyphs[96];
    int max_width = 0;
    int max_height = 0;
    for (int i = 0; i < 96; i++) {
        char text[2] = { (char)(i + siren::Font::FIRST_CHAR), '\0' };
        glyphs[i] = TTF_RenderText_Solid(ttf_font, text, COLOR_WHITE);
        if (glyphs[i] == NULL) {
            return false;
        }

        max_width = siren::max(max_width, glyphs[i]->w);
        max_height = siren::max(max_height, glyphs[i]->h);
    }

    // Render each surface glyph onto an atlas surface
    int atlas_width = siren::next_largest_power_of_two(max_width * 96);
    int atlas_height = siren::next_largest_power_of_two(max_height);
    SDL_Surface* atlas_surface = SDL_CreateRGBSurface(0, atlas_width, atlas_height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    for (int i = 0; i < 96; i++) {
        SDL_Rect dest_rect = { max_width * i, 0, glyphs[i]->w, glyphs[i]->h };
        SDL_BlitSurface(glyphs[i], NULL, atlas_surface, &dest_rect);
    }

    // Render the atlas surface onto a GL texture
    glGenTextures(1, &font->atlas);
    glBindTexture(GL_TEXTURE_2D, font->atlas);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, atlas_surface->pixels);

    // Finish setting up FontData struct
    font->glyph_width = (uint32_t)max_width;
    font->glyph_height = (uint32_t)max_height;

    // Cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    for (int i = 0; i < 96; i++) {
        SDL_FreeSurface(glyphs[i]);
    }
    SDL_FreeSurface(atlas_surface);
    TTF_CloseFont(ttf_font);

    return true;
}