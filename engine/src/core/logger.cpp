#include "logger.h"

#include "asserts.h"
#include "platform/platform.h"

#include <cstdarg>
#include <cstring>
#include <cstdio>

void logger_output(LogLevel level, const char* message, ...) {
    const char* level_prefix[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
    bool is_error = level < LOG_LEVEL_WARN;

    const int MESSAGE_LENGTH = 32000;
    char out_message[MESSAGE_LENGTH];
    memset(out_message, 0, sizeof(out_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, MESSAGE_LENGTH, message, arg_ptr);
    va_end(arg_ptr);

    char log_message[MESSAGE_LENGTH];
    sprintf(log_message, "%s%s\n", level_prefix[level], out_message);

    if (is_error) {
        platform_console_write_error(log_message, level);
    } else {
        platform_console_write(log_message, level);
    }
}

void report_assertion_failure(const char* expression, const char* message, const char* file, int line) {
    logger_output(LOG_LEVEL_FATAL, "Assertion failure: %s, message: '%s', in file: %s, line %d\n", expression, message, file, line);
}