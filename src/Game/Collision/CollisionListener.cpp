#include "CollisionListener.h"

namespace kt {

    CollisionListener::CollisionListener (spActor target): target {target} {}

    void CollisionListener::BeginContact (b2Contact * contact) {
        auto * actorA = (Actor *) contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        auto * actorB = (Actor *) contact->GetFixtureB()->GetBody()->GetUserData().pointer;

        auto * event = new CollisionEvent({actorA, actorB}, CollisionEvent::BEGIN);
        target->dispatchEvent (event);
    }

    void CollisionListener::EndContact (b2Contact * contact) {
    }

    void CollisionListener::PreSolve (b2Contact * contact, b2Manifold const * manifold) {
    }

    void CollisionListener::PostSolve (b2Contact * contact, b2ContactImpulse const * impulse) {
    }
}

/* Copyright © 2022 Aaron Alef */
