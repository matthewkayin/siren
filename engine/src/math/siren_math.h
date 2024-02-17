#pragma once

#include "defines.h"

#define SIREN_FLOAT_EPSILON 1.192092896e-07f

namespace siren {
    SIREN_API float sqrtf(float n);
    SIREN_API float fabs(float n);

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

        SIREN_INLINE float& operator[](int index) {
            return elements[index];
        }

        SIREN_INLINE const float& operator[](int index) const {
            return elements[index];
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

    union vec3 {
        float elements[3];
        struct {
            union {
                float x, r, s, u;
            };
            union {
                float y, g, t, v;
            };
            union {
                float z, b, p, w;
            };
        };

        SIREN_INLINE vec3() {
            x = 0;
            y = 0;
            z = 0;
        }

        SIREN_INLINE vec3(const float& value) {
            x = value;
            y = value;
            z = value;
        }

        SIREN_INLINE vec3(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        SIREN_INLINE float& operator[](int index) {
            return elements[index];
        }

        SIREN_INLINE const float& operator[](int index) const {
            return elements[index];
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

        SIREN_INLINE vec4(const float& value) {
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

        SIREN_INLINE float& operator[](int index) {
            return elements[index];
        }

        SIREN_INLINE const float& operator[](int index) const {
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
            return vec4(x / _length, y / _length, z / _length, w / _length);
        }
    }; // end union vec4

    typedef vec4 quat;

    union mat4 {
        float elements[16];

        SIREN_INLINE mat4() { }
        mat4(const float scaler);

        SIREN_INLINE mat4 operator*(const mat4& other) const {
            mat4 result = mat4(1.0f);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    result.elements[(i * 4) + j] = 
                        (elements[(i * 4) + 1] * other.elements[j + 0]) +
                        (elements[(i * 4) + 1] * other.elements[j + 4]) +
                        (elements[(i * 4) + 2] * other.elements[j + 8]) +
                        (elements[(i * 4) + 3] * other.elements[j + 12]);
                }
            }
        }
    };
}