#pragma once

#include "defines.h"

#define SIREN_ASSERTIONS_ENABLED

#ifdef SIREN_ASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debug_break() __debugbreak()
#else
#define debug_break() __builtin_trap()
#endif

SIREN_API void report_assertion_failure(const char* expression, const char* message, const char* file, int line);

#define SIREN_ASSERT(expr) {                                     \
    if (expr) { } else {                                         \
        report_assertion_failure(#expr, "", __FILE__, __LINE__); \
        debug_break();                                           \
    }                                                            \
}                                                                

#define SIREN_ASSERT_MESSAGE(expr, message) {                         \
    if (expr) { } else {                                              \
        report_assertion_failure(#expr, message, __FILE__, __LINE__); \
        debug_break();                                                \
    }                                                                 \
}                                                                

#ifdef _DEBUG
#define SIREN_ASSERT_DEBUG(expr) {                               \
    if (expr) { } else {                                         \
        report_assertion_failure(#expr, "", __FILE__, __LINE__); \
        debug_break();                                           \
    }                                                            \
}                                                                
#else
#define SIREN_ASSERT_DEBUG(expr)
#endif

#else
#define SIREN_ASSERT(expr)
#define SIREN_ASSERT_MESSAGE(expr, message)
#define SIREN_ASSERT_DEBUG(expr)
#endif