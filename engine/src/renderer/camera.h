#pragma once

#include "defines.h"
#include "math/vector3.h"
#include "math/matrix.h"

namespace siren {
    class Camera {
    public:
        SIREN_API Camera();
        SIREN_API vec3 get_position() const;
        SIREN_API void set_position(vec3 value);
        /*
         * Returns the front facing direction of the camera.
         * Note that if you have changed the pitch or yaw, then this value may be out of date. 
         */
        SIREN_API vec3 get_direction() const;
        SIREN_API vec3 get_up() const;
        SIREN_API vec3 get_right() const;
        SIREN_API void look_at(vec3 point);
        SIREN_API void apply_pitch(float value);
        SIREN_API void apply_yaw(float value);

        /*
         * Returns true if the camera's data has changed and the view matrix needs to be recalculated.
        */
        SIREN_API bool is_dirty() const;

        /*
         * Returns the view matrix of the camera.
         * This will recalculate the matrix if the camera's values have changed.
         */
        SIREN_API mat4 get_view_matrix();
    private:
        vec3 position;
        vec3 direction;
        vec3 up;
        float pitch;
        float yaw;

        bool dirty;
        mat4 view_matrix;
    };
}