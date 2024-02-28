#pragma once

#include "defines.h"

#include "math.h"

namespace siren {
    union ivec2 {
        int elements[2];
        struct {
            int x;
            int y;
        };

        SIREN_INLINE ivec2() {
            x = 0;
            y = 0;
        }

        SIREN_INLINE ivec2(int x, int y) {
            this->x = x;
            this->y = y;
        }

        ivec2 operator+(const ivec2& other) {
            return ivec2(x + other.x, y + other.y);
        }

        ivec2 operator-(const ivec2& other) {
            return ivec2(x - other.x, y - other.y);
        }
    };

    union vec2 {
        float elements[2];
        struct {
            union {
                float x, r, s, u;
            };
            union {
                float y, g, t, v;
            };
        };

        SIREN_INLINE vec2() {
            x = 0;
            y = 0;
        }

        SIREN_INLINE vec2(const float& value) {
            x = value;
            y = value;
        }

        SIREN_INLINE vec2(float x, float y) {
            this->x = x;
            this->y = y;
        }

        SIREN_INLINE bool operator==(const vec2& other) const {
            if (fabs(x - other.x) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            if (fabs(y - other.y) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            return true;
        }

        SIREN_INLINE vec2 operator+(const vec2& other) const {
            return vec2(x + other.x, y + other.y);
        }

        SIREN_INLINE vec2 operator-(const vec2& other) const {
            return vec2(x - other.x, y - other.y);
        }

        SIREN_INLINE vec2 operator*(const float scaler) const {
            return vec2(x * scaler, y * scaler);
        }

        SIREN_INLINE vec2 operator/(const float scaler) const {
            return vec2(x / scaler, y / scaler);
        }

        SIREN_INLINE vec2& operator+=(const vec2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        SIREN_INLINE vec2& operator-=(const vec2& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        SIREN_INLINE vec2& operator*=(const float scaler) {
            x *= scaler;
            y *= scaler;
            return *this;
        }

        SIREN_INLINE vec2& operator/=(const float scaler) {
            x /= scaler;
            y /= scaler;
            return *this;
        }
        
        SIREN_INLINE float length() const {
            return sqrtf((x * x) + (y * y));
        }

        SIREN_INLINE vec2 normalized() const {
            float _length = length();
            if (_length == 0.0f) {
                return vec2(0.0f);
            }
            return vec2(x / _length, y / _length);
        }

        SIREN_INLINE float distance_to(const vec2& other) const {
            return vec2(x - other.x, y - other.y).length();
        }

        SIREN_INLINE vec2 direction_to(const vec2& other) const {
            return vec2(x - other.x, y - other.y).normalized();
        }
    }; // end union vec2

    // vec2 constants
    static const vec2 VEC2_ZERO = vec2(0.0f, 0.0f);
    static const vec2 VEC2_UP = vec2(0.0f, -1.0f);
    static const vec2 VEC2_RIGHT = vec2(1.0f, 0.0f);
    static const vec2 VEC2_DOWN = vec2(0.0f, 1.0f);
    static const vec2 VEC2_LEFT = vec2(-1.0f, 0.0f);
}