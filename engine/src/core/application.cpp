#include "application.h"

#include "core/logger.h"
#include "core/input.h"
#include "core/resource.h"
#include "renderer/renderer.h"
#include "renderer/font.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstdlib>
#include <cstdio>

static const uint64_t FRAME_TIME = (uint64_t)(1000.0 / 60.0);

struct Application {
    bool is_running;
    uint64_t last_time;
    uint64_t last_second;
    uint32_t frames;
    uint32_t fps;
    float delta;

    siren::MouseMode mouse_mode;

    siren::Font* debug_font;

    bool (*init)();
    bool (*update)(float delta);
    bool (*render)();
};

static bool initialized = false;
static Application app;

siren::Key input_sdlk_to_key(SDL_Keycode key);

SIREN_API bool siren::application_create(siren::ApplicationConfig config) {
    if (initialized) {
        SIREN_ERROR("application_create called more than once");
        return false;
    }

    logger_init();

    // Get info out of config
    app.init = config.init;
    app.update = config.update;
    app.render = config.render;

    // Check to make sure app config was valid
    if (!app.init || !app.update || !app.render) {
        SIREN_ERROR("Application function pointers not assigned!");
        return false;
    }

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SIREN_ERROR("SDL failed to initialize: %s", SDL_GetError());
    }

    if (TTF_Init() == -1) {
        SIREN_ERROR("SDL_ttf failed to initialize: %s", TTF_GetError());
    }

    // Init subsystems
    resource_set_base_path(config.resource_path);
    input_init();
    renderer_init((RendererConfig) {
        .window_name = config.name,
        .screen_size = config.screen_size,
        .window_size = config.window_size
    });

    app.debug_font = font_system_acquire_font("font/hack.ttf", 10);

    if (!app.init()) {
        SIREN_ERROR("Application failed to initialize");
        return false;
    }

    app.is_running = true;
    app.last_time = SDL_GetTicks();
    app.last_second = app.last_time;
    app.frames = 0;
    app.fps = 0;
    app.delta = 0.0f;

    app.mouse_mode = MOUSE_MODE_VISIBLE;

    initialized = true;

    return true;
}

SIREN_API bool siren::application_run() {
    while (app.is_running) {
        // Timekeep
        uint64_t current_time = SDL_GetTicks();
        while (current_time - app.last_time < FRAME_TIME) {
            current_time = SDL_GetTicks();
        }

        app.delta = (float)(current_time - app.last_time) / 1000.0f;
        app.last_time = current_time;

        if (current_time - app.last_second >= 1000) {
            app.fps = app.frames;
            app.frames = 0;
            app.last_second += 1000;
        }

        app.frames++;

        // Poll events
        input_update();
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    app.is_running = false;
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    input_process_key(input_sdlk_to_key(e.key.keysym.sym), e.type == SDL_KEYDOWN);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    // SDL mousebuttons range from 1 to 3
                    input_process_mouse_button((MouseButton)(e.button.button - 1), e.type == SDL_MOUSEBUTTONDOWN);
                    break;
                case SDL_MOUSEMOTION:
                    input_process_mouse_motion(ivec2(e.motion.x, e.motion.y), ivec2(e.motion.xrel, e.motion.yrel));
                    break;
                case SDL_MOUSEWHEEL:
                    input_process_mouse_wheel(e.motion.y);
                    break;
            }
        }

        if (!app.update(app.delta)) {
            SIREN_ERROR("Game update failed. Shutting down.");
            app.is_running = false;
            break;
        }

        renderer_prepare_frame();
        if (!app.render()) {
            SIREN_ERROR("Game render failed. Shutting down.");
            app.is_running = false;
            break;
        }
        renderer_present_frame();
    }

    app.is_running = false;

    // Application quit
    input_quit();
    logger_quit();
    renderer_quit();

    TTF_Quit();
    SDL_Quit();

    return true;
}

uint32_t siren::application_get_fps() {
    return app.fps;
}

siren::MouseMode siren::application_get_mouse_mode() {
    return app.mouse_mode;
}

void siren::application_set_mouse_mode(siren::MouseMode mouse_mode) {
    if (app.mouse_mode == mouse_mode) {
        return;
    }

    app.mouse_mode = mouse_mode;
    if (app.mouse_mode == MOUSE_MODE_VISIBLE) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    } else {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
}