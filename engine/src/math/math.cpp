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

SIREN_INLINE int siren::next_largest_power_of_two(int number) {
    int power_of_two = 1;
    while (power_of_two < number) {
        power_of_two *= 2;
    }

    return power_of_two;
}

SIREN_INLINE siren::mat4::mat4(const float scaler) {
    memset(elements, 0, sizeof(elements));
    elements[0] = 1.0f;
    elements[5] = 1.0f;
    elements[10] = 1.0f;
    elements[15] = 1.0f;
}

SIREN_INLINE siren::mat4 siren::mat4::perspective(float fov, float aspect, float near_clip, float far_clip) {
    float half_tan_fov = tan(fov * 0.5f);

    mat4 result;
    memset(result.elements, 0, sizeof(result.elements));
    result.elements[0] = 1.0f / (aspect * half_tan_fov);
    result.elements[5] = 1.0f / half_tan_fov;
    result.elements[10] = -((far_clip - near_clip) / (far_clip - near_clip));
    result.elements[11] = -1.0f;
    result.elements[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));

    return result;
}