#ifndef CAPSTONE_PLANET_H
#define CAPSTONE_PLANET_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "src/World/World.h"

/// Number of planets per pixel
#define PLANETS_PER_PIXEL   5e-2

/// Determines gravitational pull and inertia based on planet size
#define PLANET_DENSITY      1e3

/// Scales down gravitational pull to an acceptable value
#define PLANET_GRAVITY      4e-8

/// Flag determining whether gravity wakes up spaceships
#define PLANET_WAKES_SHIP   false

namespace kt {
    using namespace oxygine;

    /// Round object with heavy mass. Exerts gravitational pull on non-planet objects
    class Planet : public Sprite {
    private:
        /// The mass of this planet
        double const mass;

    public:
        /// Create a new planet with given sprite in the given world at the given position with the given size.
        Planet (World & world, ResAnim * animation, Vector2 const & pos, float scale = 1);

        /// Exert gravitational force on non-planet objects based on mass and distance
        void update (UpdateState const & us) override;

        /// Mass formula as workaround since static bodies do not have mass
        static constexpr inline double getMass (double radius) {
            return 0.5 * M_PI * radius * radius * PLANET_DENSITY;
        }
    };

    DECLARE_SMART(Planet, spPlanet);
}

#endif //CAPSTONE_PLANET_H

/* Copyright © 2022 Aaron Alef */
