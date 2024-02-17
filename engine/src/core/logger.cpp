#include "logger.h"

#include "asserts.h"

#include <cstdarg>
#include <cstring>
#include <cstdio>

void logger_console_write(const char* message, uint8_t color);
void logger_console_write_error(const char* message, uint8_t color);

bool siren::logger_init() {
    return true;
}

void siren::logger_quit() {

}


void siren::logger_output(siren::LogLevel level, const char* message, ...) {
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
        logger_console_write_error(log_message, level);
    } else {
        logger_console_write(log_message, level);
    }
}

void report_assertion_failure(const char* expression, const char* message, const char* file, int line) {
    siren::logger_output(siren::LOG_LEVEL_FATAL, "Assertion failure: %s, message: '%s', in file: %s, line %d\n", expression, message, file, line);
}

// Platform specific console output

#ifdef SIREN_PLATFORM_WINDOWS

#include <windows.h>

void logger_console_write(const char* message, uint8_t color) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static uint8_t levels[6] = { 64, 4, 6, 2, 1, 8 };
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugStringA(message);
    uint64_t length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
}

void logger_console_write_error(const char* message, uint8_t color) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static uint8_t levels[6] = { 64, 4, 6, 2, 1, 8 };
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugStringA(message);
    uint64_t length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0);
}

#else

void platform_console_write(const char* message, uint8_t color) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[colour], message);
}

void platform_console_write_error(const char* message, uint8_t color) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    fprintf(stderr, "\033[%sm%s\033[0m", colour_strings[colour], message);
}

#endif