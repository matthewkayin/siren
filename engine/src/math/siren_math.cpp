#include "siren_math.h"

#include <cmath>
#include <cstring>

SIREN_INLINE float siren::sqrtf(float n) {
    return std::sqrtf(n);
}

SIREN_INLINE float siren::fabs(float n) {
    return std::fabs(n);
}

SIREN_INLINE siren::mat4::mat4(const float scaler) {
    memset(elements, 0, sizeof(elements));
    elements[0] = scaler;
    elements[5] = scaler;
    elements[10] = scaler;
    elements[15] = scaler;
}