#include "logger.h"

#include "asserts.h"

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix.h"

#include <cstdarg>
#include <cstring>
#include <cstdio>

void logger_console_write(const char* message, uint8_t color);
void logger_console_write_error(const char* message, uint8_t color);

static FILE* logfile;
static bool initialized = false;

bool siren::logger_init() {
    if (initialized) {
        return true;
    }

    logfile = fopen("console.log", "w");
    if (logfile == NULL) {
        SIREN_LOG_ERROR("Unable to open log file for writing");
    }

    initialized = true;

    return true;
}

void siren::logger_quit() {
    if (!initialized) {
        return;
    }

    fclose(logfile);
    initialized = false;
}

void siren::logger_output(siren::LogLevel level, const char* message, ...) {
    static char decimal_representation[] = "0123456789";
    static char convert_buffer[50];

    const char* level_prefix[4] = {"[ERROR]: ", "[WARN]: ", "[INFO]: ", "[TRACE]: "};
    bool is_error = level == LOG_LEVEL_ERROR;

    const int MESSAGE_LENGTH = 32000;
    char out_message[MESSAGE_LENGTH];
    memset(out_message, 0, sizeof(out_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    char* out_ptr = out_message;
    while (*message != '\0') {
        if (*message != '%') {
            *out_ptr = *message;
            out_ptr++;
            message++;
            continue;
        }

        message++;
        if (*message == '\0') {
            break;
        }

        switch (*message) {
            case 'c': {
                out_ptr += sprintf(out_ptr, "%c", (char)va_arg(arg_ptr, int));
                break;
            }
            case 's': {
                out_ptr += sprintf(out_ptr, "%s", va_arg(arg_ptr, char*));
                break;
            }
            case 'i': {
                out_ptr += sprintf(out_ptr, "%i", va_arg(arg_ptr, int));
                break;
            }
            case 'u': {
                out_ptr += sprintf(out_ptr, "%u", va_arg(arg_ptr, unsigned int));
                break;
            }
            case 'f': {
                out_ptr += sprintf(out_ptr, "%f", va_arg(arg_ptr, double));
                break;
            }
            case 'v': {
                message++;
                if (*message == '\0') {
                    break;
                }
                switch (*message) {
                    case '2': {
                        if (*(message + 1) == 'i') {
                            ivec2* v = va_arg(arg_ptr, ivec2*);
                            out_ptr += sprintf(out_ptr, "<%i, %i>", v->x, v->y);
                            break;
                        } else {
                            vec2* v = va_arg(arg_ptr, vec2*);
                            out_ptr += sprintf(out_ptr, "<%f, %f>", v->x, v->y);
                            break;
                        }
                    }
                    case '3': {
                        vec3* v = va_arg(arg_ptr, vec3*);
                        out_ptr += sprintf(out_ptr, "<%f, %f, %f>", v->x, v->y, v->z);
                        break;
                    }
                    case '4': {
                        vec4* v = va_arg(arg_ptr, vec4*);
                        out_ptr += sprintf(out_ptr, "<%f, %f, %f, %f>", v->x, v->y, v->z, v->w);
                        break;
                    }
                }
            } // end case v
            case 'm': {
                message++;
                if (*message == '\0') {
                    break;
                }
                switch (*message) {
                    case '4': {
                        mat4* m = va_arg(arg_ptr, mat4*);
                        for (int i = 0; i < 4; i++) {
                            out_ptr += sprintf(out_ptr, "[%f, %f, %f, %f]\n", m[0][i], m[1][i], m[2][i], m[3][i]);
                        }
                    }
                }
            }
        }

        message++;
    }
    // vsnprintf(out_message, MESSAGE_LENGTH, message, arg_ptr);
    va_end(arg_ptr);

    char log_message[MESSAGE_LENGTH];
    sprintf(log_message, "%s%s\n", level_prefix[level], out_message);

    if (is_error) {
        logger_console_write_error(log_message, level);
    } else {
        logger_console_write(log_message, level);
    }

    if (initialized) {
        fprintf(logfile, "%s", log_message);
        fflush(logfile);
    } else {
        SIREN_LOG_WARN("Called logger_output() without initializing logger. Log statement will not be written to file.");
    }
}

void report_assertion_failure(const char* expression, const char* message, const char* file, int line) {
    siren::logger_output(siren::LOG_LEVEL_ERROR, "Assertion failure: %s, message: '%s', in file: %s, line %d\n", expression, message, file, line);
}

// Platform specific console output

#ifdef SIREN_PLATFORM_WINDOWS

#include <windows.h>

void logger_console_write(const char* message, uint8_t color) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // ERROR, WARN, INFO, TRACE
    static uint8_t levels[6] = { 4, 6, 2, 8 };
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugStringA(message);
    uint64_t length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
}

void logger_console_write_error(const char* message, uint8_t color) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    // ERROR, WARN, INFO, TRACE
    static uint8_t levels[6] = { 4, 6, 2, 8 };
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugStringA(message);
    uint64_t length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0);
}

#else

void platform_console_write(const char* message, uint8_t color) {
    // ERROR,WARN,INFO,TRACE
    const char* colour_strings[] = {"1;31", "1;33", "1;32", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[colour], message);
}

void platform_console_write_error(const char* message, uint8_t color) {
    // ERROR,WARN,INFO,TRACE
    const char* colour_strings[] = {"1;31", "1;33", "1;32", "1;30"};
    fprintf(stderr, "\033[%sm%s\033[0m", colour_strings[colour], message);
}

#endif