#pragma once

#include "defines.h"

#include "math.h"
#include "matrix.h"

namespace siren {
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
            w = 1.0f;
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
            mat4 result = mat4::identity();
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

        SIREN_INLINE static quat angle_axis(vec3 axis, float angle, bool normalize) {
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
}