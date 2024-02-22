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

    siren::Font* debug_font;

    void* gamestate;

    bool (*init)(void* gamestate);
    bool (*update)(void* gamestate, float delta);
    bool (*render)(void* gamestate, float delta);
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

    SIREN_TRACE("hey");
    SIREN_INFO("hey");
    SIREN_WARN("hey");
    SIREN_ERROR("hey");

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

    if (!app.init(app.gamestate)) {
        SIREN_ERROR("Application failed to initialize");
        return false;
    }

    app.is_running = true;
    app.last_time = SDL_GetTicks();
    app.last_second = app.last_time;
    app.frames = 0;
    app.fps = 0;
    app.delta = 0.0f;

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
                    input_process_mouse_button((MouseButton)(e.button.button - 1), e.type == SDL_KEYDOWN);
                    break;
                case SDL_MOUSEMOTION:
                    input_process_mouse_motion(e.motion.x, e.motion.y);
                    break;
                case SDL_MOUSEWHEEL:
                    input_process_mouse_wheel(e.motion.y);
                    break;
            }
        }
        input_update();

        if (!app.update(app.gamestate, 0.0f)) {
            SIREN_ERROR("Game update failed. Shutting down.");
            app.is_running = false;
            break;
        }

        renderer_prepare_frame();
        if (!app.render(app.gamestate, 0.0f)) {
            SIREN_ERROR("Game render failed. Shutting down.");
            app.is_running = false;
            break;
        }
        char fps_text[16];
        sprintf(fps_text, "FPS: %u", app.fps);
        renderer_render_text(fps_text, app.debug_font, ivec2(0, 0), vec3(1.0f, 1.0f, 1.0f));
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

siren::Key input_sdlk_to_key(SDL_Keycode key) {
    switch(key) {
        case SDLK_UNKNOWN: return siren::KEY_UNKNOWN;
        case SDLK_BACKSPACE: return siren::KEY_BACKSPACE;
        case SDLK_TAB: return siren::KEY_TAB;
        case SDLK_RETURN: return siren::KEY_RETURN;
        case SDLK_ESCAPE: return siren::KEY_ESCAPE;
        case SDLK_SPACE: return siren::KEY_SPACE;
        case SDLK_EXCLAIM: return siren::KEY_EXCLAIM;
        case SDLK_QUOTEDBL: return siren::KEY_QUOTEDBL;
        case SDLK_HASH: return siren::KEY_HASH;
        case SDLK_DOLLAR: return siren::KEY_DOLLAR;
        case SDLK_PERCENT: return siren::KEY_PERCENT;
        case SDLK_AMPERSAND: return siren::KEY_AMPERSAND;
        case SDLK_QUOTE: return siren::KEY_QUOTE;
        case SDLK_LEFTPAREN: return siren::KEY_LEFTPAREN;
        case SDLK_RIGHTPAREN: return siren::KEY_RIGHTPAREN;
        case SDLK_ASTERISK: return siren::KEY_ASTERISK;
        case SDLK_PLUS: return siren::KEY_PLUS;
        case SDLK_COMMA: return siren::KEY_COMMA;
        case SDLK_MINUS: return siren::KEY_MINUS;
        case SDLK_PERIOD: return siren::KEY_PERIOD;
        case SDLK_SLASH: return siren::KEY_SLASH;
        case SDLK_0: return siren::KEY_0;
        case SDLK_1: return siren::KEY_1;
        case SDLK_2: return siren::KEY_2;
        case SDLK_3: return siren::KEY_3;
        case SDLK_4: return siren::KEY_4;
        case SDLK_5: return siren::KEY_5;
        case SDLK_6: return siren::KEY_6;
        case SDLK_7: return siren::KEY_7;
        case SDLK_8: return siren::KEY_8;
        case SDLK_9: return siren::KEY_9;
        case SDLK_COLON: return siren::KEY_COLON;
        case SDLK_SEMICOLON: return siren::KEY_SEMICOLON;
        case SDLK_LESS: return siren::KEY_LESS;
        case SDLK_EQUALS: return siren::KEY_EQUALS;
        case SDLK_GREATER: return siren::KEY_GREATER;
        case SDLK_QUESTION: return siren::KEY_QUESTION;
        case SDLK_AT: return siren::KEY_AT;
        case SDLK_LEFTBRACKET: return siren::KEY_LEFTBRACKET;
        case SDLK_BACKSLASH: return siren::KEY_BACKSLASH;
        case SDLK_RIGHTBRACKET: return siren::KEY_RIGHTBRACKET;
        case SDLK_CARET: return siren::KEY_CARET;
        case SDLK_UNDERSCORE: return siren::KEY_UNDERSCORE;
        case SDLK_BACKQUOTE: return siren::KEY_BACKQUOTE;
        case SDLK_a: return siren::KEY_a;
        case SDLK_b: return siren::KEY_b;
        case SDLK_c: return siren::KEY_c;
        case SDLK_d: return siren::KEY_d;
        case SDLK_e: return siren::KEY_e;
        case SDLK_f: return siren::KEY_f;
        case SDLK_g: return siren::KEY_g;
        case SDLK_h: return siren::KEY_h;
        case SDLK_i: return siren::KEY_i;
        case SDLK_j: return siren::KEY_j;
        case SDLK_k: return siren::KEY_k;
        case SDLK_l: return siren::KEY_l;
        case SDLK_m: return siren::KEY_m;
        case SDLK_n: return siren::KEY_n;
        case SDLK_o: return siren::KEY_o;
        case SDLK_p: return siren::KEY_p;
        case SDLK_q: return siren::KEY_q;
        case SDLK_r: return siren::KEY_r;
        case SDLK_s: return siren::KEY_s;
        case SDLK_t: return siren::KEY_t;
        case SDLK_u: return siren::KEY_u;
        case SDLK_v: return siren::KEY_v;
        case SDLK_w: return siren::KEY_w;
        case SDLK_x: return siren::KEY_x;
        case SDLK_y: return siren::KEY_y;
        case SDLK_z: return siren::KEY_z;
        case SDLK_DELETE: return siren::KEY_DELETE;
        case SDLK_CAPSLOCK: return siren::KEY_CAPSLOCK;
        case SDLK_F1: return siren::KEY_F1;
        case SDLK_F2: return siren::KEY_F2;
        case SDLK_F3: return siren::KEY_F3;
        case SDLK_F4: return siren::KEY_F4;
        case SDLK_F5: return siren::KEY_F5;
        case SDLK_F6: return siren::KEY_F6;
        case SDLK_F7: return siren::KEY_F7;
        case SDLK_F8: return siren::KEY_F8;
        case SDLK_F9: return siren::KEY_F9;
        case SDLK_F10: return siren::KEY_F10;
        case SDLK_F11: return siren::KEY_F11;
        case SDLK_F12: return siren::KEY_F12;
        case SDLK_PRINTSCREEN: return siren::KEY_PRINTSCREEN;
        case SDLK_SCROLLLOCK: return siren::KEY_SCROLLLOCK;
        case SDLK_PAUSE: return siren::KEY_PAUSE;
        case SDLK_INSERT: return siren::KEY_INSERT;
        case SDLK_HOME: return siren::KEY_HOME;
        case SDLK_PAGEUP: return siren::KEY_PAGEUP;
        case SDLK_END: return siren::KEY_END;
        case SDLK_PAGEDOWN: return siren::KEY_PAGEDOWN;
        case SDLK_RIGHT: return siren::KEY_RIGHT;
        case SDLK_LEFT: return siren::KEY_LEFT;
        case SDLK_DOWN: return siren::KEY_DOWN;
        case SDLK_UP: return siren::KEY_UP;
        case SDLK_NUMLOCKCLEAR: return siren::KEY_NUMLOCKCLEAR;
        case SDLK_KP_DIVIDE: return siren::KEY_KP_DIVIDE;
        case SDLK_KP_MULTIPLY: return siren::KEY_KP_MULTIPLY;
        case SDLK_KP_MINUS: return siren::KEY_KP_MINUS;
        case SDLK_KP_PLUS: return siren::KEY_KP_PLUS;
        case SDLK_KP_ENTER: return siren::KEY_KP_ENTER;
        case SDLK_KP_1: return siren::KEY_KP_1;
        case SDLK_KP_2: return siren::KEY_KP_2;
        case SDLK_KP_3: return siren::KEY_KP_3;
        case SDLK_KP_4: return siren::KEY_KP_4;
        case SDLK_KP_5: return siren::KEY_KP_5;
        case SDLK_KP_6: return siren::KEY_KP_6;
        case SDLK_KP_7: return siren::KEY_KP_7;
        case SDLK_KP_8: return siren::KEY_KP_8;
        case SDLK_KP_9: return siren::KEY_KP_9;
        case SDLK_KP_0: return siren::KEY_KP_0;
        case SDLK_KP_PERIOD: return siren::KEY_KP_PERIOD;
        case SDLK_APPLICATION: return siren::KEY_APPLICATION;
        case SDLK_POWER: return siren::KEY_POWER;
        case SDLK_KP_EQUALS: return siren::KEY_KP_EQUALS;
        case SDLK_F13: return siren::KEY_F13;
        case SDLK_F14: return siren::KEY_F14;
        case SDLK_F15: return siren::KEY_F15;
        case SDLK_F16: return siren::KEY_F16;
        case SDLK_F17: return siren::KEY_F17;
        case SDLK_F18: return siren::KEY_F18;
        case SDLK_F19: return siren::KEY_F19;
        case SDLK_F20: return siren::KEY_F20;
        case SDLK_F21: return siren::KEY_F21;
        case SDLK_F22: return siren::KEY_F22;
        case SDLK_F23: return siren::KEY_F23;
        case SDLK_F24: return siren::KEY_F24;
        case SDLK_EXECUTE: return siren::KEY_EXECUTE;
        case SDLK_HELP: return siren::KEY_HELP;
        case SDLK_MENU: return siren::KEY_MENU;
        case SDLK_SELECT: return siren::KEY_SELECT;
        case SDLK_STOP: return siren::KEY_STOP;
        case SDLK_AGAIN: return siren::KEY_AGAIN;
        case SDLK_UNDO: return siren::KEY_UNDO;
        case SDLK_CUT: return siren::KEY_CUT;
        case SDLK_COPY: return siren::KEY_COPY;
        case SDLK_PASTE: return siren::KEY_PASTE;
        case SDLK_FIND: return siren::KEY_FIND;
        case SDLK_MUTE: return siren::KEY_MUTE;
        case SDLK_VOLUMEUP: return siren::KEY_VOLUMEUP;
        case SDLK_VOLUMEDOWN: return siren::KEY_VOLUMEDOWN;
        case SDLK_KP_COMMA: return siren::KEY_KP_COMMA;
        case SDLK_KP_EQUALSAS400: return siren::KEY_KP_EQUALSAS400;
        case SDLK_ALTERASE: return siren::KEY_ALTERASE;
        case SDLK_SYSREQ: return siren::KEY_SYSREQ;
        case SDLK_CANCEL: return siren::KEY_CANCEL;
        case SDLK_CLEAR: return siren::KEY_CLEAR;
        case SDLK_PRIOR: return siren::KEY_PRIOR;
        case SDLK_RETURN2: return siren::KEY_RETURN2;
        case SDLK_SEPARATOR: return siren::KEY_SEPARATOR;
        case SDLK_OUT: return siren::KEY_OUT;
        case SDLK_OPER: return siren::KEY_OPER;
        case SDLK_CLEARAGAIN: return siren::KEY_CLEARAGAIN;
        case SDLK_CRSEL: return siren::KEY_CRSEL;
        case SDLK_EXSEL: return siren::KEY_EXSEL;
        case SDLK_KP_00: return siren::KEY_KP_00;
        case SDLK_KP_000: return siren::KEY_KP_000;
        case SDLK_THOUSANDSSEPARATOR: return siren::KEY_THOUSANDSSEPARATOR;
        case SDLK_DECIMALSEPARATOR: return siren::KEY_DECIMALSEPARATOR;
        case SDLK_CURRENCYUNIT: return siren::KEY_CURRENCYUNIT;
        case SDLK_CURRENCYSUBUNIT: return siren::KEY_CURRENCYSUBUNIT;
        case SDLK_KP_LEFTPAREN: return siren::KEY_KP_LEFTPAREN;
        case SDLK_KP_RIGHTPAREN: return siren::KEY_KP_RIGHTPAREN;
        case SDLK_KP_LEFTBRACE: return siren::KEY_KP_LEFTBRACE;
        case SDLK_KP_RIGHTBRACE: return siren::KEY_KP_RIGHTBRACE;
        case SDLK_KP_TAB: return siren::KEY_KP_TAB;
        case SDLK_KP_BACKSPACE: return siren::KEY_KP_BACKSPACE;
        case SDLK_KP_A: return siren::KEY_KP_A;
        case SDLK_KP_B: return siren::KEY_KP_B;
        case SDLK_KP_C: return siren::KEY_KP_C;
        case SDLK_KP_D: return siren::KEY_KP_D;
        case SDLK_KP_E: return siren::KEY_KP_E;
        case SDLK_KP_F: return siren::KEY_KP_F;
        case SDLK_KP_XOR: return siren::KEY_KP_XOR;
        case SDLK_KP_POWER: return siren::KEY_KP_POWER;
        case SDLK_KP_PERCENT: return siren::KEY_KP_PERCENT;
        case SDLK_KP_LESS: return siren::KEY_KP_LESS;
        case SDLK_KP_GREATER: return siren::KEY_KP_GREATER;
        case SDLK_KP_AMPERSAND: return siren::KEY_KP_AMPERSAND;
        case SDLK_KP_DBLAMPERSAND: return siren::KEY_KP_DBLAMPERSAND;
        case SDLK_KP_VERTICALBAR: return siren::KEY_KP_VERTICALBAR;
        case SDLK_KP_DBLVERTICALBAR: return siren::KEY_KP_DBLVERTICALBAR;
        case SDLK_KP_COLON: return siren::KEY_KP_COLON;
        case SDLK_KP_HASH: return siren::KEY_KP_HASH;
        case SDLK_KP_SPACE: return siren::KEY_KP_SPACE;
        case SDLK_KP_AT: return siren::KEY_KP_AT;
        case SDLK_KP_EXCLAM: return siren::KEY_KP_EXCLAM;
        case SDLK_KP_MEMSTORE: return siren::KEY_KP_MEMSTORE;
        case SDLK_KP_MEMRECALL: return siren::KEY_KP_MEMRECALL;
        case SDLK_KP_MEMCLEAR: return siren::KEY_KP_MEMCLEAR;
        case SDLK_KP_MEMADD: return siren::KEY_KP_MEMADD;
        case SDLK_KP_MEMSUBTRACT: return siren::KEY_KP_MEMSUBTRACT;
        case SDLK_KP_MEMMULTIPLY: return siren::KEY_KP_MEMMULTIPLY;
        case SDLK_KP_MEMDIVIDE: return siren::KEY_KP_MEMDIVIDE;
        case SDLK_KP_PLUSMINUS: return siren::KEY_KP_PLUSMINUS;
        case SDLK_KP_CLEAR: return siren::KEY_KP_CLEAR;
        case SDLK_KP_CLEARENTRY: return siren::KEY_KP_CLEARENTRY;
        case SDLK_KP_BINARY: return siren::KEY_KP_BINARY;
        case SDLK_KP_OCTAL: return siren::KEY_KP_OCTAL;
        case SDLK_KP_DECIMAL: return siren::KEY_KP_DECIMAL;
        case SDLK_KP_HEXADECIMAL: return siren::KEY_KP_HEXADECIMAL;
        case SDLK_LCTRL: return siren::KEY_LCTRL;
        case SDLK_LSHIFT: return siren::KEY_LSHIFT;
        case SDLK_LALT: return siren::KEY_LALT;
        case SDLK_LGUI: return siren::KEY_LGUI;
        case SDLK_RCTRL: return siren::KEY_RCTRL;
        case SDLK_RSHIFT: return siren::KEY_RSHIFT;
        case SDLK_RALT: return siren::KEY_RALT;
        case SDLK_RGUI: return siren::KEY_RGUI;
        case SDLK_MODE: return siren::KEY_MODE;
        case SDLK_AUDIONEXT: return siren::KEY_AUDIONEXT;
        case SDLK_AUDIOPREV: return siren::KEY_AUDIOPREV;
        case SDLK_AUDIOSTOP: return siren::KEY_AUDIOSTOP;
        case SDLK_AUDIOPLAY: return siren::KEY_AUDIOPLAY;
        case SDLK_AUDIOMUTE: return siren::KEY_AUDIOMUTE;
        case SDLK_MEDIASELECT: return siren::KEY_MEDIASELECT;
        case SDLK_WWW: return siren::KEY_WWW;
        case SDLK_MAIL: return siren::KEY_MAIL;
        case SDLK_CALCULATOR: return siren::KEY_CALCULATOR;
        case SDLK_COMPUTER: return siren::KEY_COMPUTER;
        case SDLK_AC_SEARCH: return siren::KEY_AC_SEARCH;
        case SDLK_AC_HOME: return siren::KEY_AC_HOME;
        case SDLK_AC_BACK: return siren::KEY_AC_BACK;
        case SDLK_AC_FORWARD: return siren::KEY_AC_FORWARD;
        case SDLK_AC_STOP: return siren::KEY_AC_STOP;
        case SDLK_AC_REFRESH: return siren::KEY_AC_REFRESH;
        case SDLK_AC_BOOKMARKS: return siren::KEY_AC_BOOKMARKS;
        case SDLK_BRIGHTNESSDOWN: return siren::KEY_BRIGHTNESSDOWN;
        case SDLK_BRIGHTNESSUP: return siren::KEY_BRIGHTNESSUP;
        case SDLK_DISPLAYSWITCH: return siren::KEY_DISPLAYSWITCH;
        case SDLK_KBDILLUMTOGGLE: return siren::KEY_KBDILLUMTOGGLE;
        case SDLK_KBDILLUMDOWN: return siren::KEY_KBDILLUMDOWN;
        case SDLK_KBDILLUMUP: return siren::KEY_KBDILLUMUP;
        case SDLK_EJECT: return siren::KEY_EJECT;
        case SDLK_SLEEP: return siren::KEY_SLEEP;
    }

    SIREN_WARN("Unrecognized key pressed %i", key);
    return siren::KEY_UNKNOWN;
}
