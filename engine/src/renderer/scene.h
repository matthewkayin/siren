#pragma once

#include "math/math.h"

#include "camera.h"

namespace siren {
    struct PointLight {
        vec3 position;
        vec3 color;
        float strength;
        float radius;
    };

    struct Scene {
        Camera camera;
    };

    SIREN_API Scene scene_create();
}