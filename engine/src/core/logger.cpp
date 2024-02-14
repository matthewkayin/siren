#include "logger.h"

#include "asserts.h"

#include <cstdarg>
#include <cstring>
#include <cstdio>

void logger_output(LogLevel level, const char* message, ...) {
    const char* level_prefix[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};

    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char log_message[32000];
    sprintf(log_message, "%s%s\n", level_prefix[level], out_message);

    printf("%s", log_message);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, int line) {
    logger_output(LOG_LEVEL_FATAL, "Assertion failure: %s, message: '%s', in file: %s, line %d\n", expression, message, file, line);
}