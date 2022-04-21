#ifndef CAPSTONE_WORLD_H
#define CAPSTONE_WORLD_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Game/debug/Box2DDebugDraw.h"

#define WORLD_SIZE {10, 10}

namespace kt {
    using namespace oxygine;

    class World : public Sprite {
    private:
        spBox2DDraw debugDraw;

    public:
        // Physical world
        b2World world;
        // Size of the physical world
        b2Vec2 const world_size;

        explicit World (b2Vec2 size);

        b2Vec2 wrap (b2Vec2 pos) const;

        b2Vec2 convert (Vector2 const & pos) const;

        Vector2 convert (const b2Vec2 & pos) const;

        void toggleDebugDraw ();
    };

    DECLARE_SMART (World, spWorld);

} // kt

#endif //CAPSTONE_WORLD_H

/* Copyright © 2022 Aaron Alef */
