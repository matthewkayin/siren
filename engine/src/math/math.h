#pragma once

#include "defines.h"

#include <cstring>

#define SIREN_PI 3.14159265358979323846f
#define SIREN_FLOAT_EPSILON 1.192092896e-07f
#define SIREN_DEG2RAD_MULTIPLIER SIREN_PI / 180.0f
#define SIREN_RAD2DEG_MULTIPLIER 180.0f / SIREN_PI

namespace siren {
    SIREN_API float sqrtf(float n);
    SIREN_API float fabs(float n);
    SIREN_API float sin(float radians);
    SIREN_API float cos(float radians);
    SIREN_API float tan(float radians);
    SIREN_API float acos(float radians);
    SIREN_API int max(int a, int b);
    SIREN_API int min(int a, int b);
    SIREN_API float fmax(float a, float b);
    SIREN_API float fmin(float a, float b);
    SIREN_API float clampf(float n, float lower, float upper);
    SIREN_API int next_largest_power_of_two(int number);

    SIREN_INLINE float deg_to_rad(float degrees) {
        return degrees * SIREN_DEG2RAD_MULTIPLIER;
    }

    SIREN_INLINE float rad_to_deg(float radians) {
        return radians * SIREN_RAD2DEG_MULTIPLIER;
    }
}