#ifndef CAPSTONE_NETWORK_DIRECTION_H
#define CAPSTONE_NETWORK_DIRECTION_H

#include "Network/config.h"

namespace cg {
    struct Direction {
        char accelerate: 2 = false;
        char decelerate: 2 = false;
        char rotateLeft: 2 = false;
        char rotateRight: 2 = false;

        Direction() = default;
        explicit Direction (Backend::Direction::Reader reader);

        void initialise (Backend::Direction::Builder builder) const;
    };
};

#endif //CAPSTONE_NETWORK_DIRECTION_H

/* Copyright © 2022 Aaron Alef */
