#ifndef CAPSTONE_WORLD_H
#define CAPSTONE_WORLD_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#define WORLD_SIZE {10, 10}

namespace kt {
    using namespace oxygine;

    class World : public Sprite {
    private:
        // Physical world
        b2World world;
        // Size of the physical world
        b2Vec2 world_size;

    public:
        explicit World (b2Vec2 size);
    };

    DECLARE_SMART (World, spWorld);

} // kt

#endif //CAPSTONE_WORLD_H

/* Copyright © 2022 Aaron Alef */
