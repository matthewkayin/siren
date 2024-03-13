#pragma once

#include "defines.h"

#ifndef SIREN_LOG_LEVEL
#define SIREN_LOG_LEVEL 3
#endif

namespace siren {
    enum LogLevel {
        LOG_LEVEL_ERROR = 0,
        LOG_LEVEL_WARN = 1,
        LOG_LEVEL_INFO = 2,
        LOG_LEVEL_DEBUG = 3
    };

    bool logger_init();
    void logger_quit();
    SIREN_API void logger_output(LogLevel level, const char* message, ...);
}

#define SIREN_LOG_ERROR(message, ...) logger_output(siren::LOG_LEVEL_ERROR, message, ##__VA_ARGS__);

#if SIREN_LOG_LEVEL >= 1
#define SIREN_LOG_WARN(message, ...) logger_output(siren::LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define SIREN_LOG_WARN(message, ...)
#endif

#if SIREN_LOG_LEVEL >= 2
#define SIREN_LOG_INFO(message, ...) logger_output(siren::LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define SIREN_LOG_INFO(message, ...)
#endif

#if SIREN_LOG_LEVEL >= 3
#define SIREN_LOG_DEBUG(message, ...) logger_output(siren::LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define SIREN_LOG_DEBUG(message, ...)
#endif