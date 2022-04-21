#include "World.h"

namespace kt {
    World::World (b2Vec2 size): world (b2Vec2_zero), world_size {size} {
        // Size of the World Sprite
        setSize (getStage()->getSize());

        OX_ASSERT(world_size.x > 0);
        OX_ASSERT(world_size.y > 0);
    }

    b2Vec2 World::wrap (b2Vec2 pos) const {
        while (pos.x < 0) pos.x += world_size.x;
        while (pos.y < 0) pos.y += world_size.y;
        pos.x = fmod (pos.x, world_size.x);
        pos.y = fmod (pos.y, world_size.y);

        return pos;
    }

    b2Vec2 World::convert (Vector2 const & pos) const {
        return {
            pos.x * getSize().x / world_size.x,
            pos.y * getSize().y / world_size.y
        };
    }

    Vector2 World::convert (b2Vec2 const & pos) const {
        return {
            pos.x * world_size.x / getSize().x,
            pos.y * world_size.y / getSize().y
        };
    }
}

/* Copyright © 2022 Aaron Alef */
