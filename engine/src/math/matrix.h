#pragma once

#include "defines.h"

#include "math.h"
#include "vector3.h"
#include "vector4.h"

#include <cstring>

namespace siren {
    struct mat4 {
        vec4 columns[4];

        SIREN_INLINE mat4(float value = 0.0f) {
            columns[0] = vec4(value, 0.0f, 0.0f, 0.0f);
            columns[1] = vec4(0.0f, value, 0.0f, 0.0f);
            columns[2] = vec4(0.0f, 0.0f, value, 0.0f);
            columns[3] = vec4(0.0f, 0.0f, 0.0f, value);
        }

        SIREN_INLINE vec4& operator[](uint32_t index) {
            return columns[index];
        }

        SIREN_INLINE const vec4& operator[](uint32_t index) const {
            return columns[index];
        }

        SIREN_INLINE mat4 operator*(const mat4& other) const {
            mat4 result;

            for (uint32_t row = 0; row < 4; row++) {
                for (uint32_t col = 0; col < 4; col++) {
                    result[col][row] = (columns[0][row] * other[col][0]) +
                                       (columns[1][row] * other[col][1]) +
                                       (columns[2][row] * other[col][2]) +
                                       (columns[3][row] * other[col][3]);
                }
            }

            return result;
        }

        SIREN_INLINE static mat4 orthographic(float left, float right, float bottom, float top, float near, float far) {
            mat4 result(1.0f);
            result[0][0] = 2.0f / (right - left);
            result[1][1] = 2.0f / (top - bottom);
            result[2][2] = -2.0f / (far - near);
            result[3][0] = -(right + left) / (right - left);
            result[3][1] = -(top + bottom) / (top - bottom);
            result[3][2] = -(far + near) / (far - near);
            
            return result;
        }

        SIREN_INLINE static mat4 perspective(float fov, float aspect, float near, float far) {
            float half_tan_fov = tan(fov * 0.5f);

            mat4 result(0.0f);
            result[0][0] = 1.0f / (aspect * half_tan_fov);
            result[1][1] = 1.0f / half_tan_fov;
            result[2][2] = -(far + near) / (far - near);
            result[2][3] = -1.0f;
            result[3][2] = -(2.0f * far * near) / (far - near);

            return result;
        }

        SIREN_INLINE static mat4 look_at(vec3 position, vec3 target, vec3 up) {
            vec3 z_axis = (target - position).normalized();
            vec3 x_axis = vec3::cross(z_axis, up).normalized();
            vec3 y_axis = vec3::cross(x_axis, z_axis);

            mat4 result(1.0f);

            result[0][0] = x_axis.x;
            result[1][0] = x_axis.y;
            result[2][0] = x_axis.z;

            result[0][1] = y_axis.x;
            result[1][1] = y_axis.y;
            result[2][1] = y_axis.z;

            result[0][2] = -z_axis.x;
            result[1][2] = -z_axis.y;
            result[2][2] = -z_axis.z;

            result[3][0] = -vec3::dot(x_axis, position);
            result[3][1] = -vec3::dot(y_axis, position);
            result[3][2] = vec3::dot(z_axis, position);

            return result;
        }

        SIREN_INLINE static mat4 translate(vec3 position) {
            mat4 result(1.0f);
            result[3][0] = position.x;
            result[3][1] = position.y;
            result[3][2] = position.z;
            return result;
        }

        SIREN_INLINE static mat4 rotate(float angle, vec3 axis) {
            float cos_theta = cos(angle);
            float sin_theta = sin(angle);

            mat4 result(1.0f);
            result[0][0] = cos_theta + ((axis.x * axis.x) * (1 - cos_theta));
            result[0][1] = (axis.y * axis.x * (1 - cos_theta)) + (axis.z * sin_theta);
            result[0][2] = (axis.z * axis.x * (1 - cos_theta)) - (axis.y * sin_theta);
            result[1][0] = (axis.x * axis.y * (1 - cos_theta)) - (axis.z * sin_theta);
            result[1][1] = cos_theta + (axis.y * axis.y * (1 - cos_theta));
            result[1][2] = (axis.z * axis.y * (1 - cos_theta)) + (axis.x * sin_theta);
            result[2][0] = (axis.x * axis.z * (1 - cos_theta)) + (axis.y * sin_theta);
            result[2][1] = (axis.y * axis.z * (1 - cos_theta)) - (axis.x * sin_theta);
            result[2][2] = cos_theta + (axis.x * axis.x * (1 - cos_theta));

            return result;
        }

        SIREN_INLINE static mat4 scale(vec3 value) {
            mat4 result(1.0f);
            result[0][0] = value.x;
            result[1][1] = value.y;
            result[2][2] = value.z;

            return result;
        }
    };
}