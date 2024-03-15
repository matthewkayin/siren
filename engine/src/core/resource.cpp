#include "resource.h"

static std::string resource_path;

void siren::resource_set_base_path(const char* path) {
    resource_path = std::string(path);
}

const std::string siren::resource_get_base_path() {
    return resource_path;
}