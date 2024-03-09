#pragma once

#include "defines.h"

#include "math.h"

namespace siren {
    union vec4 {
        float elements[4];
        struct {
            union {
                float x, r, s;
            };
            union {
                float y, g, t;
            };
            union {
                float z, b, p;
            };
            union {
                float w, a, q;
            };
        };

        SIREN_INLINE vec4() {
            x = 0;
            y = 0;
            z = 0;
            w = 0;
        }

        SIREN_INLINE vec4(const float value) {
            x = value;
            y = value;
            z = value;
            w = value;
        }

        SIREN_INLINE vec4(float x, float y, float z, float w) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        SIREN_INLINE float& operator[](uint32_t index) {
            return elements[index];
        }

        SIREN_INLINE const float& operator[](uint32_t index) const {
            return elements[index];
        }

        SIREN_INLINE bool operator==(const vec4& other) const {
            if (fabs(x - other.x) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            if (fabs(y - other.y) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            if (fabs(z - other.z) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            if (fabs(w - other.w) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            return true;
        }

        SIREN_INLINE vec4 operator+(const vec4& other) const {
            return vec4(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        SIREN_INLINE vec4 operator-(const vec4& other) const {
            return vec4(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        SIREN_INLINE vec4 operator*(const float scaler) const {
            return vec4(x * scaler, y * scaler, z * scaler, w * scaler);
        }

        SIREN_INLINE vec4 operator/(const float scaler) const {
            return vec4(x / scaler, y / scaler, z / scaler, w / scaler);
        }

        SIREN_INLINE vec4& operator+=(const vec4& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        SIREN_INLINE vec4& operator-=(const vec4& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        SIREN_INLINE vec4& operator*=(const float scaler) {
            x *= scaler;
            y *= scaler;
            z *= scaler;
            w *= scaler;
            return *this;
        }

        SIREN_INLINE vec4& operator/=(const float scaler) {
            x /= scaler;
            y /= scaler;
            z /= scaler;
            w /= scaler;
            return *this;
        }

        SIREN_INLINE float length() const {
            return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        }

        SIREN_INLINE vec4 normalized() const {
            float _length = length();
            if (_length == 0.0f) {
                return vec4(0.0f);
            }
            return vec4(x / _length, y / _length, z / _length, w / _length);
        }
    }; // end union vec4
}