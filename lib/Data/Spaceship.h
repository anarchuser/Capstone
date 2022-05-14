#ifndef CAPSTONE_SPACESHIP_H
#define CAPSTONE_SPACESHIP_H

#include <string>
#include <array>

struct Spaceship {
    std::string username;
    std::array <float, 2> position;
    std::array <float, 2> velocity;
};

#endif //CAPSTONE_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
