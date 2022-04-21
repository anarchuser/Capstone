#ifndef CAPSTONE_PLANET_H
#define CAPSTONE_PLANET_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Game/World/World.h"

/// Number of planets per pixel
#define PLANETS_PER_PIXEL   5e-2

#define PLANET_DENSITY      1e3

#define PLANET_GRAVITY      2e7

namespace kt {
    using namespace oxygine;

    class Planet : public Sprite {
    public:
        Planet (World & world, oxygine::ResAnim * animation, Vector2 const & pos, float scale = 1);

        void update (UpdateState const & us) override;
    };

    DECLARE_SMART(Planet, spPlanet);
}

#endif //CAPSTONE_PLANET_H

/* Copyright © 2022 Aaron Alef */
