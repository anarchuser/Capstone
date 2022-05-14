#ifndef CAPSTONE_DIRECTION_H
#define CAPSTONE_DIRECTION_H

#include "Server/generated/synchro.capnp.h"

namespace cg {
    struct Direction {
        char accelerate: 2 = false;
        char decelerate: 2 = false;
        char rotateLeft: 2 = false;
        char rotateRight: 2 = false;

        Direction() = default;
        explicit Direction (Synchro::Direction::Reader reader);

        void initialise (Synchro::Direction::Builder builder) const;
    };
};

#endif //CAPSTONE_DIRECTION_H

/* Copyright © 2022 Aaron Alef */
