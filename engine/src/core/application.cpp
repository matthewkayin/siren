#include "application.h"

#include "platform/platform.h"
#include "core/logger.h"
#include "core/input.h"
#include "core/siren_memory.h"

#include <cstdlib>

struct ApplicationState {
    siren::Application* app;
    bool is_running;
    bool is_suspended;
    siren::PlatformState platform;
    int width;
    int height;
    double last_time;
};

static bool initialized = false;
static ApplicationState app_state;

SIREN_API bool siren::application_create(siren::Application* app) {
    if (initialized) {
        SIREN_ERROR("application_create called more than once");
        return false;
    }

    app_state.app = app;

    // Init subsystems
    memory_init();
    logger_init();
    input_init();

    SIREN_FATAL("testing %f", 3.14f);
    SIREN_ERROR("testing %f", 3.14f);
    SIREN_WARN("testing %f", 3.14f);
    SIREN_INFO("testing %f", 3.14f);
    SIREN_DEBUG("testing %f", 3.14f);
    SIREN_TRACE("testing %f", 3.14f);

    app_state.is_running = true;
    app_state.is_suspended = false;

    if (!platform_init(&app_state.platform, app->config.name, app->config.x, app->config.y, app->config.width, app->config.height)) {
        return false;
    }

    if (!app_state.app->init || !app_state.app->update || !app_state.app->render || !app_state.app->on_resize) {
        SIREN_FATAL("Application function pointers not assigned!");
        return false;
    }

    if (!app_state.app->init(app_state.app->gamestate)) {
        SIREN_FATAL("Application failed to initialize");
        return false;
    }

    app_state.app->on_resize(app_state.app->gamestate, app_state.width, app_state.height);

    initialized = true;

    return true;
}

SIREN_API bool siren::application_run() {
    char* memory_usage_str = memory_create_usage_str();
    SIREN_INFO(memory_usage_str);
    free(memory_usage_str);

    while (app_state.is_running) {
        if (!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = false;
        }

        if (!app_state.is_suspended) {
            if (!app_state.app->update(app_state.app->gamestate, 0.0f)) {
                SIREN_ERROR("Game update failed. Shutting down.");
                app_state.is_running = false;
                break;
            }

            if (!app_state.app->render(app_state.app->gamestate, 0.0f)) {
                SIREN_ERROR("Game render failed. Shutting down.");
                app_state.is_running = false;
                break;
            }

            input_update();
        }
    }

    app_state.is_running = false;

    input_quit();
    logger_quit();
    memory_quit();
    platform_quit(&app_state.platform);

    return true;
}