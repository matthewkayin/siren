#include "font.h"

#include "core/logger.h"
#include "core/resource.h"
#include "math/math.h"
#include "container/hashtable.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstring>
#include <cstdio>

const uint16_t MAX_FONTS = 16;
const int FIRST_CHAR = 32;

struct FontSystemState {
    siren::Hashtable* fonts;
    uint16_t font_count;
};

static bool initialized = false;
static FontSystemState state;

void siren::font_system_init() {
    if (initialized) {
        return;
    }

    state.fonts = siren::hashtable_create(sizeof(Font), MAX_FONTS);
    state.font_count = 0;

    initialized = true;
}

void siren::font_system_quit() {
    if (!initialized) {
        return;
    }

    siren::hashtable_destroy(state.fonts);

    initialized = false;
}

siren::Font* siren::font_system_acquire_font(const char* path, uint16_t size) {
    static const SDL_Color COLOR_WHITE = { 255, 255, 255, 255 };

    char key[256];
    sprintf(key, "%s:%u", path, size);

    // Check if font has been loaded
    Font* font;
    siren::hashtable_get_ptr(state.fonts, key, (void**)&font);
    if (font != nullptr) {
        return font;
    }

    // Check that there is room for this font in the font system
    if (state.font_count == MAX_FONTS) {
        SIREN_ERROR("No space left to allocate a new font.");
        return nullptr;
    }

    // Begin creating a new font
    font = (Font*)malloc(sizeof(Font));

    // Load the font
    char full_path[128];
    sprintf(full_path, "%s%s", resource_get_base_path(), path);

    TTF_Font* ttf_font = TTF_OpenFont(full_path, size);
    if (ttf_font == NULL) {
        SIREN_ERROR("Unable to open font at path %s. SDL Error: %s", path, TTF_GetError());
        return nullptr;
    }

    // Render each glyph to a surface
    SDL_Surface* glyphs[96];
    int max_width = 0;
    int max_height = 0;
    for (int i = 0; i < 96; i++) {
        char text[2] = { (char)(i + FIRST_CHAR), '\0' };
        glyphs[i] = TTF_RenderText_Blended(ttf_font, text, COLOR_WHITE);
        if (glyphs[i] == NULL) {
            return nullptr;
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

    // Place the font in the hashtable
    siren::hashtable_set_ptr(state.fonts, key, (void**)&font);
    state.font_count++;

    // Cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    for (int i = 0; i < 96; i++) {
        SDL_FreeSurface(glyphs[i]);
    }
    SDL_FreeSurface(atlas_surface);
    TTF_CloseFont(ttf_font);

    return font;
}