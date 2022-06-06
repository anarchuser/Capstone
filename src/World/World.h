#ifndef CAPSTONE_WORLD_H
#define CAPSTONE_WORLD_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "src/debug/Box2DDebugDraw.h"
#include "src/Collision/CollisionListener.h"

/// Size of the physical world. Determines x/y ratio and size of objects contained
#define WORLD_SIZE {10, 10}

/// Number of iterations performed per second. Adjust based on network load
#define FPS 60.0

namespace kt {
    using namespace oxygine;

    /// Wrapper around the physical world. Responsible for displaying and updating objects in the world
    class World : public Sprite {
    private:
        /// Toggled with `` ` ``. Overlays rendered sprites with box2d primitives for debugging purposes
        spBox2DDraw debugDraw;

        /// Turn debug view on or off. Invoked by pressing `` ` ``
        void toggleDebugDraw ();

    public:
        /// Physical world, i.e., box2d world responsible for the physics in the game
        b2World world {b2Vec2_zero};
        /// Size of physical world
        b2Vec2 const world_size;

        /// Construct a new world with the given background and size
        World (ResAnim * background, b2Vec2 size);

        /// Update all physical bodies and their corresponding sprites
        void update (UpdateState const & updateState) override;

        /// Modulates the given vector to be within [0, world_size], i.e., ensures no object leaves the world
        b2Vec2 wrap (b2Vec2 pos) const;

        /// Convert an oxygine vector to a box2d vector
        b2Vec2 convert (Vector2 const & pos) const;

        /// Convert a box2d vector to an oxygine vector
        Vector2 convert (const b2Vec2 & pos) const;

        /// Overload to ensure all children have their position normalised
        void addChild (spActor child);
    };

    DECLARE_SMART (World, spWorld);

} // kt

#endif //CAPSTONE_WORLD_H

/* Copyright © 2022 Aaron Alef */
