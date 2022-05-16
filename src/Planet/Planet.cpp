#include "Planet.h"

namespace kt {
    Planet::Planet (World & world, ResAnim * animation, Vector2 const & pos, float scale) {
        setName ("Planet");
        attachTo (& world);
        auto world_pos = world.convert (pos);
        OX_ASSERT (world.wrap (world_pos) == world.wrap (world.wrap (world_pos)));

        setResAnim (animation);
        setAnchor (0.5, 0.5);
        setTouchChildrenEnabled (false);

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = world_pos;
        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

        auto * body = world.world.CreateBody (& bodyDef);
        setUserData (body);
        setPosition (world.convert (world_pos));
        setScale (scale);

        b2CircleShape circleShape;
        circleShape.m_radius = 0.5f * scale;

        b2FixtureDef circleFixture;
        circleFixture.shape = & circleShape;
        circleFixture.density = PLANET_DENSITY;
        circleFixture.friction = 1.0;
        body->CreateFixture (& circleFixture);
    }

    void Planet::update (UpdateState const & us) {
        auto * body = (b2Body *) getUserData();
        auto * world = safeCast<World *> (getParent());
        spActor current_actor = world->getFirstChild();

        while (current_actor) {
            auto * actor_body = (b2Body *) current_actor->getUserData();

            // Apply force to existing foreign dynamic bodies only
            if (actor_body && body != actor_body && actor_body->GetType() == b2_dynamicBody) {
                auto space = world->world_size;
                auto direction = body->GetWorldCenter () - actor_body->GetWorldCenter ();

                if (direction.x > 0.5 * space.x) {
                    direction.x -= space.x;
                } else if (direction.x < -0.5 * space.x) {
                    direction.x += space.x;
                }
                if (direction.y > 0.5 * space.y) {
                    direction.y -= space.y;
                } else if (direction.y < -0.5 * space.y) {
                    direction.y += space.y;
                }

                auto force = body->GetMass() * PLANET_GRAVITY / direction.Normalize ();
                actor_body->ApplyLinearImpulseToCenter (force * direction, PLANET_WAKES_SHIP);
            }

            current_actor = current_actor->getNextSibling();
        }

        Actor::update (us);
    }
}

/* Copyright © 2022 Aaron Alef */
