#include "Planet.h"

ph::Planet::Planet (b2World * world, oxygine::ResAnim * animation, Vector2 const & pos, float scale) {
    setResAnim (animation);
    setAnchor (0.5, 0.5);
    setTouchChildrenEnabled (false);

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = ph::convert (pos);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    auto * body = world->CreateBody (& bodyDef);
    setUserData (body);
    setScale (scale);

    b2CircleShape circleShape;
    circleShape.m_radius = 0.5f * scale;

    b2FixtureDef circleFixture;
    circleFixture.shape = & circleShape;
    circleFixture.density = DENSITY_PLANET;
    circleFixture.friction = 1.0;
    body->CreateFixture (& circleFixture);
}

void ph::Planet::update (oxygine::UpdateState const & us) {
    auto * body = (b2Body *) getUserData();
    spActor actor = getParent()->getFirstChild();
    while (actor) {
        auto * actor_body = (b2Body *) actor->getUserData();

        /// Apply force to dynamic bodies only
        if (actor_body->GetType() == b2_dynamicBody) {
            auto direction = body->GetWorldCenter () - actor_body->GetWorldCenter ();
            auto force = GRAVITY_PLANET / direction.Normalize();

            actor_body->ApplyLinearImpulseToCenter (force * direction, false);
        }

        spActor next = actor->getNextSibling();
        actor = next;
    }

    Actor::update (us);
}

/* Copyright © 2022 Aaron Alef */
