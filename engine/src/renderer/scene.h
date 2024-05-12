#pragma once

#include "math/math.h"

#include "camera.h"

#include <vector>

namespace siren {
    struct Light {
        vec3 position;
        vec3 color;
    };

    struct Scene {
        Camera camera;
        std::vector<Light> lights;
    };

    SIREN_API Scene scene_create();
}