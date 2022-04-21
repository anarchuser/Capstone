#include "World.h"

namespace kt {
    World::World (b2Vec2 size): world (b2Vec2_zero), world_size {size} {
        // Size of the World Sprite
        setSize (getStage()->getSize());
    }
}

/* Copyright © 2022 Aaron Alef */
