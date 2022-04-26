#ifndef CAPSTONE_COLLISIONLISTENER_H
#define CAPSTONE_COLLISIONLISTENER_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "CollisionEvent.h"

namespace kt {

    class CollisionListener: public b2ContactListener {
    private:
        spActor target;

    public:
        explicit CollisionListener (spActor target);

        void BeginContact (b2Contact * contact) override;
        void EndContact   (b2Contact * contact) override;

        void PreSolve  (b2Contact * contact, b2Manifold const * manifold) override;
        void PostSolve (b2Contact * contact, b2ContactImpulse const * impulse) override;
    };

} // kt

#endif //CAPSTONE_COLLISIONLISTENER_H

/* Copyright © 2022 Aaron Alef */
