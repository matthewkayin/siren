#include "font.h"

#include "renderer_types.h"
#include "core/logger.h"
#include "core/resource.h"
#include "math/math.h"
#include "container/hashtable.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstring>
#include <cstdio>

const uint16_t MAX_FONTS = 16;
const int FIRST_CHAR = 32;

struct FontSystemState {
    siren::Hashtable* font_name_to_id;
    siren::FontData font_data[MAX_FONTS];
    uint16_t font_count;
};

static bool initialized = false;
static FontSystemState state;

void siren::font_system_init() {
    if (initialized) {
        return;
    }

    state.font_name_to_id = siren::hashtable_create(sizeof(FontId), MAX_FONTS);
    memset(state.font_name_to_id->data, FONT_ID_INVALID, state.font_name_to_id->element_count * state.font_name_to_id->element_size);
    state.font_count = 0;

    initialized = true;
}

void siren::font_system_quit() {
    if (!initialized) {
        return;
    }

    siren::hashtable_destroy(state.font_name_to_id);

    initialized = false;
}

siren::FontId siren::font_system_acquire_font(const char* path, uint16_t size) {
    static const SDL_Color COLOR_WHITE = { 255, 255, 255, 255 };

    char key[256];
    sprintf(key, "%s:%u", path, size);

    // Check if font has been loaded
    FontId font_id; 
    siren::hashtable_get(state.font_name_to_id, key, &font_id);
    if (font_id != FONT_ID_INVALID) {
        return font_id;
    }

    // Check that there is room for this font in the font system
    if (state.font_count == MAX_FONTS) {
        SIREN_ERROR("No space left to allocate a new font.");
        return FONT_ID_INVALID;
    }

    // Begin creating a new font
    FontData font_data;

    // Load the font
    char full_path[128];
    sprintf(full_path, "%s%s", resource_get_base_path(), path);

    TTF_Font* ttf_font = TTF_OpenFont(full_path, size);
    if (ttf_font == NULL) {
        SIREN_ERROR("Unable to open font at path %s. SDL Error: %s", path, TTF_GetError());
        return FONT_ID_INVALID;
    }

    // Render each glyph to a surface
    SDL_Surface* glyphs[96];
    int max_width = 0;
    int max_height = 0;
    for (int i = 0; i < 96; i++) {
        char text[2] = { (char)(i + FIRST_CHAR), '\0' };
        glyphs[i] = TTF_RenderText_Blended(ttf_font, text, COLOR_WHITE);
        if (glyphs[i] == NULL) {
            return FONT_ID_INVALID;
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
    glGenTextures(1, &font_data.atlas);
    glBindTexture(GL_TEXTURE_2D, font_data.atlas);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, atlas_surface->pixels);

    // Finish setting up FontData struct
    font_data.glyph_width = (uint32_t)max_width;
    font_data.glyph_height = (uint32_t)max_height;

    // Determine the font id
    font_id = state.font_count;
    siren::hashtable_set(state.font_name_to_id, key, &font_id);
    state.font_count++;
    state.font_data[font_id] = font_data;

    // Cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    for (int i = 0; i < 96; i++) {
        SDL_FreeSurface(glyphs[i]);
    }
    SDL_FreeSurface(atlas_surface);
    TTF_CloseFont(ttf_font);

    return font_id;
}