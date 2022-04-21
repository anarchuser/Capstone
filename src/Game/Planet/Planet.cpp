#include "Planet.h"

namespace kt {
    Planet::Planet (World & world, oxygine::ResAnim * animation, Vector2 const & pos, float scale) {
        auto world_pos = world.convert (pos);
        OX_ASSERT (world.wrap (world_pos) == world.wrap (world.wrap (world_pos)));
        OX_ASSERT (world_pos == world.wrap (world_pos));

        setPosition (pos); //TODO: factor this out into update function?

        setResAnim (animation);
        setAnchor (0.5, 0.5);
        setTouchChildrenEnabled (false);

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = world_pos;
        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

        auto * body = world.world.CreateBody (& bodyDef);
        setUserData (body);
        setScale (scale);

        b2CircleShape circleShape;
        circleShape.m_radius = 0.5f * scale;

        b2FixtureDef circleFixture;
        circleFixture.shape = & circleShape;
        circleFixture.density = PLANET_DENSITY;
        circleFixture.friction = 1.0;
        body->CreateFixture (& circleFixture);
    }
}

/* Copyright © 2022 Aaron Alef */
