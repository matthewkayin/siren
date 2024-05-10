#include "math.h"

#include <cmath>
#include <cstring>

SIREN_INLINE float siren::sqrtf(float n) {
    return std::sqrtf(n);
}

SIREN_INLINE float siren::fabs(float n) {
    return std::fabs(n);
}

SIREN_INLINE float siren::sin(float radians) {
    return std::sinf(radians);
}

SIREN_INLINE float siren::cos(float radians) {
    return std::cosf(radians);
}

SIREN_INLINE float siren::tan(float radians) {
    return std::tanf(radians);
}

SIREN_INLINE float siren::acos(float radians) {
    return std::acosf(radians);
}

SIREN_INLINE int siren::max(int a, int b) {
    return a > b ? a : b;
}

SIREN_INLINE int siren::min(int a, int b) {
    return a < b ? a : b;
}

SIREN_INLINE float siren::fmax(float a, float b) {
    return a > b ? a : b;
}

SIREN_INLINE float siren::fmin(float a, float b) {
    return a < b ? a : b;
}

SIREN_INLINE float siren::clampf(float n, float lower, float upper) {
    if (n < lower) {
        return lower;
    }
    if (n > upper) {
        return upper;
    }
    return n;
}

SIREN_INLINE int siren::next_largest_power_of_two(int number) {
    int power_of_two = 1;
    while (power_of_two < number) {
        power_of_two *= 2;
    }

    return power_of_two;
}