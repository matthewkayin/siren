#pragma once

#include "defines.h"

#include "math.h"

namespace siren {
    union vec3 {
        float elements[3];
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        };
        struct {
            float s, t, p;
        };
        struct {
            float u, v, w;
        };

        SIREN_INLINE vec3() {
            x = 0;
            y = 0;
            z = 0;
        }

        SIREN_INLINE vec3(float value) {
            x = value;
            y = value;
            z = value;
        }

        SIREN_INLINE vec3(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        SIREN_INLINE bool operator==(const vec3& other) const {
            if (fabs(x - other.x) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            if (fabs(y - other.y) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            if (fabs(z - other.z) > SIREN_FLOAT_EPSILON) {
                return false;
            }
            return true;
        }

        SIREN_INLINE vec3 operator+(const vec3& other) const {
            return vec3(x + other.x, y + other.y, z + other.z);
        }

        SIREN_INLINE vec3 operator-(const vec3& other) const {
            return vec3(x - other.x, y - other.y, z - other.z);
        }

        SIREN_INLINE vec3 operator*(const float scaler) const {
            return vec3(x * scaler, y * scaler, z * scaler);
        }

        SIREN_INLINE vec3 operator/(const float scaler) const {
            return vec3(x / scaler, y / scaler, z / scaler);
        }

        SIREN_INLINE vec3& operator+=(const vec3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        
        SIREN_INLINE vec3& operator-=(const vec3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        SIREN_INLINE vec3& operator*=(const float scaler) {
            x *= scaler;
            y *= scaler;
            z *= scaler;
            return *this;
        }
        
        SIREN_INLINE vec3& operator/=(const float scaler) {
            x /= scaler;
            y /= scaler;
            z /= scaler;
            return *this;
        }

        SIREN_INLINE float length() const {
            return sqrtf((x * x) + (y * y) + (z * z));
        }

        SIREN_INLINE vec3 normalized() const {
            float _length = length();
            if (_length == 0.0f) {
                return vec3(0.0f);
            }
            return vec3(x / _length, y / _length, z / _length);
        }

        SIREN_INLINE float distance_to(const vec3& other) const {
            return vec3(x - other.x, y - other.y, z - other.z).length();
        }

        SIREN_INLINE vec3 direction_to(const vec3& other) const {
            return vec3(x - other.x, y - other.y, z - other.z).normalized();
        }

        SIREN_INLINE static float dot(const vec3& a, const vec3& b) {
            return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
        }

        SIREN_INLINE static vec3 cross(const vec3& a, const vec3& b) {
            return vec3((a.y * b.z) - (a.z * b.y),
                        (a.z * b.x) - (a.x * b.z),
                        (a.x * b.y) - (a.y * b.x));
        }
    }; // end union vec3

    // vec3 constants
    static const vec3 VEC3_ZERO = vec3(0.0f, 0.0f, 0.0f);
    static const vec3 VEC3_UP = vec3(0.0f, -1.0f, 0.0f);
    static const vec3 VEC3_DOWN = vec3(0.0f, 1.0f, 0.0f);
    static const vec3 VEC3_LEFT = vec3(-1.0f, 0.0f, 0.0f);
    static const vec3 VEC3_RIGHT = vec3(1.0f, 0.0f, 0.0f);
    static const vec3 VEC3_FORWARD = vec3(0.0f, 0.0f, -1.0f);
    static const vec3 VEC3_BACK = vec3(0.0f, 0.0f, 1.0f);
}