#include "math.h"

#include <cstring>

SIREN_INLINE int siren::max(int a, int b) {
    return a > b ? a : b;
}

SIREN_INLINE int siren::min(int a, int b) {
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