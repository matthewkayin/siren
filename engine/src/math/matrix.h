#pragma once

#include "defines.h"

#include "math.h"
#include "vector3.h"

#include <cstring>
#include <cstdio>

namespace siren {
    struct mat4 {
        float elements[16];

        static mat4 identity() {
            mat4 _identity;
            memset(_identity.elements, 0, sizeof(_identity.elements));
            _identity.elements[0] = 1.0f;
            _identity.elements[5] = 1.0f;
            _identity.elements[10] = 1.0f;
            _identity.elements[15] = 1.0f;
            return _identity;
        }

        SIREN_INLINE mat4 operator*(const mat4& other) const {
            mat4 result = mat4::identity();

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    result.elements[(i * 4) + j] = 
                        (elements[(i * 4) + 0] * other.elements[j + 0]) +
                        (elements[(i * 4) + 1] * other.elements[j + 4]) +
                        (elements[(i * 4) + 2] * other.elements[j + 8]) +
                        (elements[(i * 4) + 3] * other.elements[j + 12]);
                }
            }

            return result;
        }

        SIREN_INLINE static mat4 orthographic(float left, float right, float bottom, float top, float near_clip, float far_clip) {
            mat4 result = mat4::identity();

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

        static mat4 perspective(float fov, float aspect, float near_clip, float far_clip) {
            float half_tan_fov = tan(fov * 0.5f);

            mat4 result;
            memset(result.elements, 0, sizeof(result.elements));
            result.elements[0] = 1.0f / (aspect * half_tan_fov);
            result.elements[5] = 1.0f / half_tan_fov;
            result.elements[10] = -((far_clip + near_clip) / (far_clip - near_clip));
            result.elements[11] = -1.0f;
            result.elements[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));

            return result;
        }

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

        SIREN_INLINE float determinant() const {
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

            float o0 = (t0 * elements[5] + t3 * elements[9] + t4 * elements[13]) -
                (t1 * elements[5] + t2 * elements[9] + t5 * elements[13]);
            float o1 = (t1 * elements[1] + t6 * elements[9] + t9 * elements[13]) -
                (t0 * elements[1] + t7 * elements[9] + t8 * elements[13]);
            float o2 = (t2 * elements[1] + t7 * elements[5] + t10 * elements[13]) -
                (t3 * elements[1] + t6 * elements[5] + t11 * elements[13]);
            float o3 = (t5 * elements[1] + t8 * elements[5] + t11 * elements[9]) -
                (t4 * elements[1] + t9 * elements[5] + t10 * elements[9]);

            return 1.0f / (elements[0] * o0 + elements[4] * o1 + elements[8] * o2 + elements[12] * o3);
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

        SIREN_INLINE static mat4 translate(vec3 position) {
            mat4 result = mat4::identity();
            result.elements[12] = position.x;
            result.elements[13] = position.y;
            result.elements[14] = position.z;
            return result;
        }

        SIREN_INLINE static mat4 scale(vec3 scale) {
            mat4 result = mat4::identity();
            result.elements[0] = scale.x;
            result.elements[5] = scale.y;
            result.elements[10] = scale.z;
            return result;
        }

        SIREN_INLINE static mat4 rotate(vec3 euler_angles) {
            mat4 result = mat4::identity();
            float cos_x = cos(euler_angles.x);
            float sin_x = sin(euler_angles.x);

            result.elements[5] = cos_x;
            result.elements[6] = sin_x;
            result.elements[9] = -sin_x;
            result.elements[10] = cos_x;

            float cos_y = cos(euler_angles.y);
            float sin_y = sin(euler_angles.y);

            result.elements[0] = cos_y;
            result.elements[2] = -sin_y;
            result.elements[8] = sin_y;
            result.elements[10] = cos_y;

            float cos_z = cos(euler_angles.z);
            float sin_z = sin(euler_angles.z);

            result.elements[0] = cos_z;
            result.elements[1] = sin_z;
            result.elements[4] = -sin_z;
            result.elements[5] = cos_z;

            return result;
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
            return vec3(elements[0], elements[4], elements[8]).normalized();
        }

        SIREN_INLINE void to_string(char* buffer, size_t buffer_size) const {
            for (int i = 0; i < 4; i++) {
                sprintf(buffer, "[%f %f %f %f]", 
                elements[(i * 4) + 0],
                elements[(i * 4) + 1],
                elements[(i * 4) + 2],
                elements[(i * 4) + 3]
                );
            }
        }
    };
}