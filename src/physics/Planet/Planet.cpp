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

/* Copyright © 2022 Aaron Alef */
