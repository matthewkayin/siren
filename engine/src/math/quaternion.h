#pragma once

#include "defines.h"

#include "math/math.h"
#include "math/matrix.h"

namespace siren {
    struct quat {
        float x;
        float y;
        float z;
        float w;

        SIREN_INLINE quat() {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
            w = 1.0f;
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
            float _normal = normal();
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
            mat4 result(1.0f);
            quat n = normalized();

            result[0][0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
            result[0][1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
            result[0][2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

            result[1][0] = 2.0f * n.x * n.y * 2.0f * n.z * n.w;
            result[1][1] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
            result[1][2] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

            result[2][0] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
            result[2][1] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
            result[2][2] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

            return result;
        }

        SIREN_INLINE mat4 to_mat4_around_center(vec3 center) const {
            mat4 result;

            result[0][0] = (x * x) - (y * y) - (z * z) + (w * w);
            result[0][1] = 2.0f * ((x * y) + (z * w));
            result[0][2] = 2.0f * ((x * z) - (y * w));
            result[0][3] = center.x - center.x * result[0][0] - center.y * result[0][1] - center.z * result[0][2];

            result[1][0] = 2.0f * ((x * y) - (z * w));
            result[1][1] = -(x * x) + (y * y) - (z * z) + (w * w);
            result[1][2] = 2.0f * ((y * z) + (x * w));
            result[1][3] = center.y - center.x * result[1][0] - center.y * result[1][1] - center.z * result[1][2];

            result[2][0] = 2.0f * ((x * z) + (y * w));
            result[2][1] = 2.0f * ((y * z) - (x * w));
            result[2][2] = -(x * x) - (y * y) + (z * z) + (w * w);
            result[2][3] = center.z - center.x * result[2][0] - center.y * result[2][1] - center.z * result[2][2];

            result[3][0] = 0.0f;
            result[3][1] = 0.0f;
            result[3][2] = 0.0f;
            result[3][3] = 1.0f;

            return result;
        }

        SIREN_INLINE static quat from_axis_angle(vec3 axis, float angle, bool normalize) {
            const float half_angle = 0.5f * angle;
            float sin_half_angle = sin(half_angle);
            float cos_half_angle = cos(half_angle);

            quat result = quat(sin_half_angle * axis.x, sin_half_angle * axis.y, sin_half_angle * axis.z, cos_half_angle);
            return normalize ? result.normalized() : result;
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
}