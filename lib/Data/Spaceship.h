#ifndef CAPSTONE_STRUCT_SPACESHIP_H
#define CAPSTONE_STRUCT_SPACESHIP_H

#include <string>
#include <array>

namespace cg {
    struct Spaceship {
        std::string username;
        std::array<float, 2> position = {0};
        std::array<float, 2> velocity = {0};
        float angle;
    };
}

#endif //CAPSTONE_STRUCT_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
