#pragma once

#include "defines.h"

#define SIREN_LOG_WARN_ENABLED 1
#define SIREN_LOG_INFO_ENABLED 1
#define SIREN_LOG_TRACE_ENABLED 1

#if SIREN_RELEASE == 1
#define SIREN_LOG_TRACE_ENABLED 0
#endif

namespace siren {
    enum LogLevel {
        LOG_LEVEL_ERROR = 0,
        LOG_LEVEL_WARN = 1,
        LOG_LEVEL_INFO = 2,
        LOG_LEVEL_TRACE = 3
    };

    bool logger_init();
    void logger_quit();
    SIREN_API void logger_output(LogLevel level, const char* message, ...);
}

#ifndef SIREN_ERROR
#define SIREN_ERROR(message, ...) logger_output(siren::LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if SIREN_LOG_WARN_ENABLED == 1
#define SIREN_WARN(message, ...) logger_output(siren::LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define SIREN_WARN(message, ...)
#endif

#if SIREN_LOG_INFO_ENABLED == 1
#define SIREN_INFO(message, ...) logger_output(siren::LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define SIREN_INFO(message, ...)
#endif

#if SIREN_LOG_TRACE_ENABLED == 1
#define SIREN_TRACE(message, ...) logger_output(siren::LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define SIREN_TRACE(message, ...)
#endif