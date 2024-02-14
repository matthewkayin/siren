#pragma once

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if SIREN_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

enum LogLevel {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
};

SIREN_API void logger_output(LogLevel level, const char* message, ...);

#define SIREN_FATAL(message, ...) logger_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef SIREN_ERROR
#define SIREN_ERROR(message, ...) logger_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define SIREN_WARN(message, ...) logger_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define SIREN_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define SIREN_INFO(message, ...) logger_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define SIREN_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define SIREN_DEBUG(message, ...) logger_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define SIREN_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define SIREN_TRACE(message, ...) logger_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define SIREN_TRACE(message, ...)
#endif