#include "CollisionListener.h"

namespace kt {

    void CollisionListener::BeginContact (b2Contact * contact) {
        auto * actorA = (Sprite *) contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        auto * actorB = (Sprite *) contact->GetFixtureB()->GetBody()->GetUserData().pointer;

        // Fire corresponding collision events to all actors that are not planets (and therefore spaceships)
        if (! actorB->isName ("Planet")) {
            actorB->dispatchEvent (new CollisionEvent (actorA, CollisionEvent::BEGIN));
        }
        if (! actorA->isName ("Planet")) {
            actorA->dispatchEvent (new CollisionEvent (actorB, CollisionEvent::BEGIN));
        }
    }

    void CollisionListener::EndContact (b2Contact * contact) {
    }

    void CollisionListener::PreSolve (b2Contact * contact, b2Manifold const * manifold) {
    }

    void CollisionListener::PostSolve (b2Contact * contact, b2ContactImpulse const * impulse) {
    }
}

/* Copyright © 2022 Aaron Alef */
