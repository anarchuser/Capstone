#ifndef CAPSTONE_NETWORK_SPACESHIP_H
#define CAPSTONE_NETWORK_SPACESHIP_H

#include "Network/config.h"

#include <string>
#include <array>

namespace cg {
    /// A helper struct to store x and y data
    struct vector2 {
        /// Value on the x-axis
        float x = 0;
        /// Value on the y-axis
        float y = 0;
    };

    /// Spaceship status struct in correspondence with Backend::Spaceship
    struct Spaceship {
        /// The owner of the ship
        std::string username;
        /// The position of the spaceship
        vector2 position;
        /// The velocity of the spaceship
        vector2 velocity;
        /// The direction the spaceship is looking, in radians
        float angle;
        /// The amount of remaining hit points of the spaceship
        int health;

        /// Construct a new spaceship from the given parameters
        explicit Spaceship (
                std::string username,
                vector2 position = {0},
                vector2 velocity = {0},
                float angle = 0,
                int health = 100);
        /// Construct a new spaceship from the given Spaceship::Reader
        explicit Spaceship (Backend::Spaceship::Reader reader);

        /// Use this spaceship to initialise the given Spaceship::Builder
        void initialise (Backend::Spaceship::Builder builder) const;
    };
}

#endif //CAPSTONE_NETWORK_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
