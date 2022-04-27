#ifndef CAPSTONE_COLLISIONLISTENER_H
#define CAPSTONE_COLLISIONLISTENER_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "CollisionEvent.h"

namespace kt {

    /// Handles collision events issued by the physical world
    class CollisionListener: public b2ContactListener {
    public:
        /// Creates a new listener to be added to the world
        CollisionListener ();

        /// Callback when two objects collide
        void BeginContact (b2Contact * contact) override;
        /// Callback when two objects stop touching
        void EndContact   (b2Contact * contact) override;

        /// Callback before two objects touch (their bounding boxes come into range)
        void PreSolve  (b2Contact * contact, b2Manifold const * manifold) override;
        /// Callback after a collision has been resolved
        void PostSolve (b2Contact * contact, b2ContactImpulse const * impulse) override;
    };

} // kt

#endif //CAPSTONE_COLLISIONLISTENER_H

/* Copyright © 2022 Aaron Alef */
