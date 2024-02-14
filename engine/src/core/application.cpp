#include "application.h"

#include "game_types.h"
#include "platform/platform.h"
#include "logger.h"
#include "core/siren_memory.h"

#include <cstdlib>

struct ApplicationState {
    Game* game;
    bool is_running;
    bool is_suspended;
    PlatformState platform;
    int width;
    int height;
    double last_time;
};

static bool initialized = false;
static ApplicationState app_state;

SIREN_API bool application_create(Game* game) {
    if (initialized) {
        SIREN_ERROR("application_create called more than once");
        return false;
    }

    app_state.game = game;

    // Init subsystems
    logger_init();

    SIREN_FATAL("testing %f", 3.14f);
    SIREN_ERROR("testing %f", 3.14f);
    SIREN_WARN("testing %f", 3.14f);
    SIREN_INFO("testing %f", 3.14f);
    SIREN_DEBUG("testing %f", 3.14f);
    SIREN_TRACE("testing %f", 3.14f);

    app_state.is_running = true;
    app_state.is_suspended = false;

    if (!platform_init(&app_state.platform, game->app_config.name, game->app_config.x, game->app_config.y, game->app_config.width, game->app_config.height)) {
        return false;
    }

    if (!app_state.game->init(app_state.game)) {
        SIREN_FATAL("Game failed to initialize");
        return false;
    }

    app_state.game->on_resize(app_state.game, app_state.width, app_state.height);

    initialized = true;

    return true;
}

SIREN_API bool application_run() {
    char* memory_usage_str = create_memory_usage_str();
    SIREN_INFO(memory_usage_str);
    free(memory_usage_str);

    while (app_state.is_running) {
        if (!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = false;
        }

        if (!app_state.is_suspended) {
            if (!app_state.game->update(app_state.game, 0.0f)) {
                SIREN_ERROR("Game update failed. Shutting down.");
                app_state.is_running = false;
                break;
            }

            if (!app_state.game->render(app_state.game, 0.0f)) {
                SIREN_ERROR("Game render failed. Shutting down.");
                app_state.is_running = false;
                break;
            }
        }
    }

    app_state.is_running = false;

    platform_quit(&app_state.platform);

    return true;
}