#include <core/logger.h>
#include <core/asserts.h>

int main() {
    SIREN_FATAL("testing %f", 3.14f);
    SIREN_ERROR("testing %f", 3.14f);
    SIREN_WARN("testing %f", 3.14f);
    SIREN_INFO("testing %f", 3.14f);
    SIREN_DEBUG("testing %f", 3.14f);
    SIREN_TRACE("testing %f", 3.14f);

    SIREN_ASSERT(1 == 0);

    return 0;
}