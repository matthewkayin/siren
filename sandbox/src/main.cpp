#include <core/logger.h>
#include <core/asserts.h>
#include <platform/platform.h>

int main() {
    SIREN_FATAL("testing %f", 3.14f);
    SIREN_ERROR("testing %f", 3.14f);
    SIREN_WARN("testing %f", 3.14f);
    SIREN_INFO("testing %f", 3.14f);
    SIREN_DEBUG("testing %f", 3.14f);
    SIREN_TRACE("testing %f", 3.14f);

    PlatformState platform_state;
    if (platform_init(&platform_state, "sandbox", 100, 100, 1280, 720)) {
        while (true) {
            platform_pump_messages(&platform_state);
        }
    }
    platform_quit(&platform_state);

    return 0;
}