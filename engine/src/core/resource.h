#pragma once

#include <string>

namespace siren {
    void resource_set_base_path(const char* path);
    const std::string resource_get_base_path();

    static const uint32_t RESOURCE_HANDLE_NULL = UINT32_MAX;
}