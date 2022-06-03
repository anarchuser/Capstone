#ifndef CAPSTONE_NETWORK_SPACESHIP_H
#define CAPSTONE_NETWORK_SPACESHIP_H

#include "Network/config.h"

#include <string>
#include <array>

namespace cg {
    struct vector2 {
        float x = 0;
        float y = 0;
    };

    struct Spaceship {
        std::string username;
        vector2 position;
        vector2 velocity;
        float angle;
        int health;

        explicit Spaceship (Backend::Spaceship::Reader reader);
        explicit Spaceship (
                std::string username,
                vector2 position = {0},
                vector2 velocity = {0},
                float angle = 0,
                int health = 100);

        void initialise (Backend::Spaceship::Builder builder) const;
    };
}

#endif //CAPSTONE_NETWORK_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
