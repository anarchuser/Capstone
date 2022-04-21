#ifndef CAPSTONE_PLANET_H
#define CAPSTONE_PLANET_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

/// Number of planets per pixel
#define PLANETS_PER_PIXEL   1e-6

namespace kt {
    using namespace oxygine;

    class Planet : public Sprite {
    public:
        Planet (b2World * world, oxygine::ResAnim * animation, Vector2 const & pos, float scale = 1);
    };

    DECLARE_SMART(Planet, spPlanet);
}

#endif //CAPSTONE_PLANET_H

/* Copyright © 2022 Aaron Alef */
