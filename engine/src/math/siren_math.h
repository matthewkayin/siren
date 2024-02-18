#pragma once

#include "defines.h"

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

    struct mat4 {
        float elements[16];

        SIREN_INLINE mat4() {}
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

            return result;
        }

        SIREN_INLINE static mat4 orthographic(float left, float right, float bottom, float top, float near_clip, float far_clip) {
            mat4 result = mat4(1.0f);

            float lr = 1.0f / (left - right);
            float bt = 1.0f / (bottom - top);
            float nf = 1.0f / (near_clip - far_clip);

            result.elements[0] = -2.0f * lr;
            result.elements[5] = -2.0f * bt;
            result.elements[10] = 2.0f * nf;
            result.elements[12] = (left + right) * lr;
            result.elements[13] = (top + bottom) * bt;
            result.elements[14] = (far_clip + near_clip) * nf;

            return result;
        }

        static mat4 perspective(float fov, float aspect, float near_clip, float far_clip);

        SIREN_INLINE static mat4 look_at(vec3 position, vec3 target, vec3 up) {
            mat4 result;
            vec3 z_axis = (target - position).normalized();
            vec3 x_axis = vec3::cross(z_axis, up).normalized();
            vec3 y_axis = vec3::cross(x_axis, z_axis);

            result.elements[0] = x_axis.x;
            result.elements[1] = y_axis.x;
            result.elements[2] = -z_axis.x;
            result.elements[3] = 0;

            result.elements[4] = x_axis.y;
            result.elements[5] = y_axis.y;
            result.elements[6] = -z_axis.y;
            result.elements[7] = 0;

            result.elements[8] = x_axis.z;
            result.elements[9] = y_axis.z;
            result.elements[10] = -z_axis.z;
            result.elements[11] = 0;

            result.elements[12] = -vec3::dot(x_axis, position);
            result.elements[13] = -vec3::dot(y_axis, position);
            result.elements[14] = vec3::dot(z_axis, position);
            result.elements[15] = 1.0f;

            return result;
        }

        SIREN_INLINE mat4 transposed() const {
            mat4 result;
            result.elements[0] = elements[0];
            result.elements[1] = elements[4];
            result.elements[2] = elements[8];
            result.elements[3] = elements[12];
            result.elements[4] = elements[1];
            result.elements[5] = elements[5];
            result.elements[6] = elements[9];
            result.elements[7] = elements[13];
            result.elements[8] = elements[2];
            result.elements[9] = elements[6];
            result.elements[10] = elements[10];
            result.elements[11] = elements[14];
            result.elements[12] = elements[3];
            result.elements[13] = elements[7];
            result.elements[14] = elements[11];
            result.elements[15] = elements[15];
            return result;
        }

        SIREN_INLINE mat4 inversed() const {
            float t0 = elements[10] * elements[15];
            float t1 = elements[14] * elements[11];
            float t2 = elements[6] * elements[15];
            float t3 = elements[14] * elements[7];
            float t4 = elements[6] * elements[11];
            float t5 = elements[10] * elements[7];
            float t6 = elements[2] * elements[15];
            float t7 = elements[14] * elements[3];
            float t8 = elements[2] * elements[11];
            float t9 = elements[10] * elements[3];
            float t10 = elements[2] * elements[7];
            float t11 = elements[6] * elements[3];
            float t12 = elements[8] * elements[13];
            float t13 = elements[12] * elements[9];
            float t14 = elements[4] * elements[13];
            float t15 = elements[12] * elements[5];
            float t16 = elements[4] * elements[9];
            float t17 = elements[8] * elements[5];
            float t18 = elements[0] * elements[13];
            float t19 = elements[12] * elements[1];
            float t20 = elements[0] * elements[9];
            float t21 = elements[8] * elements[1];
            float t22 = elements[0] * elements[5];
            float t23 = elements[4] * elements[1];

            mat4 result;

            result.elements[0] = (t0 * elements[5] + t3 * elements[9] + t4 * elements[13]) - (t1 * elements[5] + t2 * elements[9] + t5 * elements[13]);
            result.elements[1] = (t1 * elements[1] + t6 * elements[9] + t9 * elements[13]) - (t0 * elements[1] + t7 * elements[9] + t8 * elements[13]);
            result.elements[2] = (t2 * elements[1] + t7 * elements[5] + t10 * elements[13]) - (t3 * elements[1] + t6 * elements[5] + t11 * elements[13]);
            result.elements[3] = (t5 * elements[1] + t8 * elements[5] + t11 * elements[9]) - (t4 * elements[1] + t9 * elements[5] + t10 * elements[9]);

            float d = 1.0f / (elements[0] * result.elements[0] + elements[4] * result.elements[1] + elements[8] * result.elements[2] + elements[12] * result.elements[3]);

            result.elements[0] = d * result.elements[0];
            result.elements[1] = d * result.elements[1];
            result.elements[2] = d * result.elements[2];
            result.elements[3] = d * result.elements[3];
            result.elements[4] = d * ((t1 * elements[4] + t2 * elements[8] + t5 * elements[12]) - (t0 * elements[4] + t3 * elements[8] + t4 * elements[12]));
            result.elements[5] = d * ((t0 * elements[0] + t7 * elements[8] + t8 * elements[12]) - (t1 * elements[0] + t6 * elements[8] + t9 * elements[12]));
            result.elements[6] = d * ((t3 * elements[0] + t6 * elements[4] + t11 * elements[12]) - (t2 * elements[0] + t7 * elements[4] + t10 * elements[12]));
            result.elements[7] = d * ((t4 * elements[0] + t9 * elements[4] + t10 * elements[8]) - (t5 * elements[0] + t8 * elements[4] + t11 * elements[8]));
            result.elements[8] = d * ((t12 * elements[7] + t15 * elements[11] + t16 * elements[15]) - (t13 * elements[7] + t14 * elements[11] + t17 * elements[15]));
            result.elements[9] = d * ((t13 * elements[3] + t18 * elements[11] + t21 * elements[15]) - (t12 * elements[3] + t19 * elements[11] + t20 * elements[15]));
            result.elements[10] = d * ((t14 * elements[3] + t19 * elements[7] + t22 * elements[15]) - (t15 * elements[3] + t18 * elements[7] + t23 * elements[15]));
            result.elements[11] = d * ((t17 * elements[3] + t20 * elements[7] + t23 * elements[11]) - (t16 * elements[3] + t21 * elements[7] + t22 * elements[11]));
            result.elements[12] = d * ((t14 * elements[10] + t17 * elements[14] + t13 * elements[6]) - (t16 * elements[14] + t12 * elements[6] + t15 * elements[10]));
            result.elements[13] = d * ((t20 * elements[14] + t12 * elements[2] + t19 * elements[10]) - (t18 * elements[10] + t21 * elements[14] + t13 * elements[2]));
            result.elements[14] = d * ((t18 * elements[6] + t23 * elements[14] + t15 * elements[2]) - (t22 * elements[14] + t14 * elements[2] + t19 * elements[6]));
            result.elements[15] = d * ((t22 * elements[10] + t16 * elements[2] + t21 * elements[6]) - (t20 * elements[6] + t23 * elements[10] + t17 * elements[2]));
            
            return result;
        }

        SIREN_INLINE static mat4 translation(vec3 position) {
            mat4 result = mat4(1.0f);
            result.elements[12] = position.x;
            result.elements[13] = position.y;
            result.elements[14] = position.z;
            return result;
        }

        SIREN_INLINE static mat4 scale(vec3 scale) {
            mat4 result = mat4(1.0f);
            result.elements[0] = scale.x;
            result.elements[5] = scale.y;
            result.elements[10] = scale.z;
            return result;
        }

        SIREN_INLINE static mat4 euler_x(float radians) {
            mat4 result = mat4(1.0f);
            float c = cos(radians);
            float s = sin(radians);

            result.elements[5] = c;
            result.elements[6] = s;
            result.elements[9] = -s;
            result.elements[10] = c;

            return result;
        }

        SIREN_INLINE static mat4 euler_y(float radians) {
            mat4 result = mat4(1.0f);
            float c = cos(radians);
            float s = sin(radians);

            result.elements[0] = c;
            result.elements[2] = -s;
            result.elements[8] = s;
            result.elements[10] = c;

            return result;
        }

        SIREN_INLINE static mat4 euler_z(float radians) {
            mat4 result = mat4(1.0f);
            float c = cos(radians);
            float s = sin(radians);

            result.elements[0] = c;
            result.elements[1] = s;
            result.elements[4] = -s;
            result.elements[5] = c;

            return result;
        }

        SIREN_INLINE static mat4 euler_xyz(float x_radians, float y_radians, float z_radians) {
            return (euler_x(x_radians) * euler_y(y_radians)) * euler_z(z_radians);
        }

        SIREN_INLINE vec3 forward() const {
            return vec3(-elements[2], -elements[6], -elements[10]).normalized();
        }

        SIREN_INLINE vec3 backward() const {
            return vec3(elements[2], elements[6], elements[10]).normalized();
        }

        SIREN_INLINE vec3 up() const {
            return vec3(elements[1], elements[5], elements[9]).normalized();
        }

        SIREN_INLINE vec3 down() const {
            return vec3(-elements[1], -elements[5], -elements[9]).normalized();
        }

        SIREN_INLINE vec3 left() const {
            return vec3(-elements[0], -elements[4], -elements[8]).normalized();
        }

        SIREN_INLINE vec3 right() const {
            return vec3(-elements[0], -elements[4], -elements[8]).normalized();
        }
    };

    union quat {
        float elements[4];
        struct {
            float x;
            float y;
            float z;
            float w;
        };

        SIREN_INLINE quat() {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
            w = 0.0f;
        }

        SIREN_INLINE quat(const float value) {
            x = value;
            y = value;
            z = value;
            w = value;
        }

        SIREN_INLINE quat(float x, float y, float z, float w) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        SIREN_INLINE float normal() const {
            return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        }

        SIREN_INLINE quat normalized() const {
            float _normal;
            return quat(x / _normal, y / _normal, z / _normal, w / _normal);
        }

        SIREN_INLINE quat conjugate() const {
            return quat(-x, -y, -z, w);
        }

        SIREN_INLINE quat inverse() const {
            return conjugate().normalized();
        }

        SIREN_INLINE bool operator==(const quat& other) const {
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

        SIREN_INLINE quat operator+(const quat& other) const {
            return quat(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        SIREN_INLINE quat operator-(const quat& other) const {
            return quat(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        SIREN_INLINE quat operator*(const quat& other) const {
            quat result;

            result.x = x * other.w +
                       y * other.z -
                       z * other.y +
                       w * other.x;
            
            result.y = -x * other.z +
                        y * other.w +
                        z * other.x +
                        w * other.y;

            result.z = x * other.y - 
                       y * other.x +
                       z * other.w +
                       w * other.z;

            result.w = -x * other.x - 
                        y * other.y -
                        z * other.z +
                        w * other.w;

            return result;
        }

        SIREN_INLINE static float dot(const quat& a, const quat& b) {
            return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
        }

        SIREN_INLINE mat4 to_mat4() const {
            mat4 result = mat4(1.0f);
            quat n = normalized();

            result.elements[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
            result.elements[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
            result.elements[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

            result.elements[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
            result.elements[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
            result.elements[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

            result.elements[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
            result.elements[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
            result.elements[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

            return result;
        }

        SIREN_INLINE mat4 to_rotation_matrix(vec3 center) {
            mat4 result;

            result.elements[0] = (x * x) - (y * y) - (z * z) + (w * w);
            result.elements[1] = 2.0f * ((x * y) + (z * w));
            result.elements[2] = 2.0f * ((x * z) - (y * w));
            result.elements[3] = center.x - center.x * result.elements[0] - center.y * result.elements[1] - center.z * result.elements[2];

            result.elements[4] = 2.0f * ((x * y) - (z * w));
            result.elements[5] = -(x * x) + (y * y) - (z * z) + (w * w);
            result.elements[6] = 2.0f * ((y * z) + (x * w));
            result.elements[7] = center.y - center.x * result.elements[4] - center.y * result.elements[5] - center.z * result.elements[6];

            result.elements[8] = 2.0f * ((x * z) + (y * w));
            result.elements[9] = 2.0f * ((y * z) - (x * w));
            result.elements[10] = -(x * x) - (y * y) + (z * z) + (w * w);
            result.elements[11] = center.z - center.x * result.elements[8] - center.y * result.elements[9] - center.z * result.elements[10];

            result.elements[12] = 0.0f;
            result.elements[13] = 0.0f;
            result.elements[14] = 0.0f;
            result.elements[15] = 1.0f;

            return result;
        }

        SIREN_INLINE static quat from_axis_angle(vec3 axis, float angle, bool normalize) {
            float half_angle = 0.5f * angle;
            float _sin = sin(half_angle);
            float _cos = cos(half_angle);

            quat result = quat(_sin * axis.x, _sin * axis.y, _sin * axis.z, _cos);
            if (normalize) {
                result = result.normalized();
            }

            return result;
        }

        SIREN_INLINE static quat slerp(quat from, quat to, float percentage) {
            quat result;

            quat v0 = from.normalized();
            quat v1 = to.normalized();
            float _dot = dot(v0, v1);

            if (_dot < 0.0f) {
                v1.x = -v1.x;
                v1.y = -v1.y;
                v1.z = -v1.z;
                v1.w = -v1.w;
                _dot = -_dot;
            }

            const float DOT_THRESHOLD = 0.9995f;
            if (_dot > DOT_THRESHOLD) {
                // if the inputs are too close together, just lerp and normalize the result
                return quat(
                    v0.x + ((v1.x - v0.x) * percentage),
                    v0.y + ((v1.y - v0.y) * percentage),
                    v0.z + ((v1.z - v0.z) * percentage),
                    v0.w + ((v1.w - v0.w) * percentage)
                ).normalized();
            }

            float theta_0 = acos(_dot);
            float theta = theta_0 * percentage;
            float sin_theta = sin(theta);
            float sin_theta_0 = sin(theta_0);

            float s0 = cos(theta) - _dot * sin_theta / sin_theta_0;
            float s1 = sin_theta / sin_theta_0;

            return quat(
                (v0.x * s0) + (v1.x * s1),
                (v0.y * s0) + (v1.y * s1),
                (v0.z * s0) + (v1.z * s1),
                (v0.w * s0) + (v1.w * s1)
            );
        }
    };

    SIREN_INLINE float deg_to_rad(float degrees) {
        return degrees * SIREN_DEG2RAD_MULTIPLIER;
    }

    SIREN_INLINE float rad_to_deg(float radians) {
        return radians * SIREN_RAD2DEG_MULTIPLIER;
    }
}