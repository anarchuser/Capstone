#ifndef CAPSTONE_NETWORK_DIRECTION_H
#define CAPSTONE_NETWORK_DIRECTION_H

#include "Network/config.h"

namespace cg {
    /// Stores directional data in one byte
    struct Direction {
        /// Acceleration flag. Negative means do not update
        char accelerate: 2 = false;
        /// Deceleration flag. Negative means do not update
        char decelerate: 2 = false;
        /// Rotate left flag. Negative means do not update
        char rotateLeft: 2 = false;
        /// Rotate right flag. Negative means do not update
        char rotateRight: 2 = false;

        /// Default constructor initialising all values to false
        Direction() = default;
        /// Construct a new Direction struct from the given Direction::Reader
        explicit Direction (Backend::Direction::Reader reader);

        /// Initialise the given Direction::Builder object
        void initialise (Backend::Direction::Builder builder) const;
    };
};

#endif //CAPSTONE_NETWORK_DIRECTION_H

/* Copyright © 2022 Aaron Alef */
