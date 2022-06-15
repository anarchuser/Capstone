#ifndef CAPSTONE_NETWORK_DIRECTION_H
#define CAPSTONE_NETWORK_DIRECTION_H

#include "Network/config.h"

namespace cg {
    /// Stores directional data in one byte.
    struct Direction {
        /// Tell an entity to accelerate
        char accelerate: 2 = false;
        /// Tell an entity to decelerate
        char decelerate: 2 = false;
        /// Tell an entity to rotate left
        char rotateLeft: 2 = false;
        /// Tell en entity to rotate right
        char rotateRight: 2 = false;

        /// Construct a direction with all values set to false
        Direction() = default;
        /// Construct a new Direction struct from the given Direction::Reader
        explicit Direction (Backend::Direction::Reader reader);

        /// Initialise the given Direction::Builder object
        void initialise (Backend::Direction::Builder builder) const;
    };
};

#endif //CAPSTONE_NETWORK_DIRECTION_H

/* Copyright © 2022 Aaron Alef */
