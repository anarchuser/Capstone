#ifndef CAPSTONE_ENV_H
#define CAPSTONE_ENV_H

#include <unistd.h>
#include <limits>
#include <string>

#include "Network/config.h"

#include "oxygine-framework.h"

#define MAX_HOSTNAME_SIZE 256

static inline std::string const & username () {
    static const std::string username = std::getenv ("USER") ?: "username";
    return username;
}
static inline std::string const & hostname () {
    static const auto hostname = [] () ->std::string {
        char buffer [MAX_HOSTNAME_SIZE] = {0};
        if (gethostname (buffer, sizeof (buffer)) == 0) return buffer;
        return username();
    }();
    return hostname;
}

#endif //CAPSTONE_ENV_H

/* Copyright © 2022 Aaron Alef */
