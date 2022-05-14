#ifndef CAPSTONE_STRUCT_SPACESHIP_H
#define CAPSTONE_STRUCT_SPACESHIP_H

#include <string>
#include <array>

#include "Server/generated/synchro.capnp.h"

namespace cg {
    using vector2 = std::array <float, 2>;

    struct Spaceship {
        std::string username;
        vector2 position = {0};
        vector2 velocity = {0};
        float angle;

        explicit Spaceship (Synchro::Spaceship::Reader reader);
        Spaceship (std::string username, vector2 position, vector2 velocity, float angle);

        void initialise (Synchro::Spaceship::Builder builder) const;
    };
}

#endif //CAPSTONE_STRUCT_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
