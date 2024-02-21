#include "resource.h"

#include <cstring>

static char resource_path[64];

void siren::resource_set_base_path(const char* path) {
    strcpy_s(resource_path, 64, path);
}

const char* siren::resource_get_base_path() {
    return resource_path;
}