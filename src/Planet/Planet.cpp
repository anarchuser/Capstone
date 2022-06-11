#include "Planet.h"

namespace kt {
    Planet::Planet (World & world, ResAnim * animation, Vector2 const & pos, float scale): mass {getMass (0.5 * scale)} {
        setName ("Planet");
        attachTo (& world);
        auto world_pos = world.convert (pos);
        OX_ASSERT (world.wrap (world_pos) == world.wrap (world.wrap (world_pos)));

        // initialise the planet sprite
        setResAnim (animation);
        setAnchor (0.5, 0.5);
        setTouchChildrenEnabled (false);

        // Configure the properties of the planet
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position = world_pos;
        bodyDef.userData.pointer = (uintptr_t) this;

        // Create the body in the world
        auto * body = world.world.CreateBody (& bodyDef);
        setUserData (body);
        setPosition (world.convert (world_pos));
        setScale (scale);

        // Configure the body to be a circle
        b2CircleShape circleShape;
        circleShape.m_radius = 0.5f * scale;

        // Set the physical properties
        b2FixtureDef circleFixture;
        circleFixture.shape = & circleShape;
        circleFixture.density = PLANET_DENSITY;
        circleFixture.friction = 1.0;
        body->CreateFixture (& circleFixture);
    }

    void Planet::update (UpdateState const & updateState) {
        auto * body = (b2Body *) getUserData();
        auto * world = safeCast<World *> (getParent());

        // Iterate over all actors, applying gravity to all dynamic objects
        spActor current_actor = world->getFirstChild();
        while (current_actor) {
            auto * actor_body = (b2Body *) current_actor->getUserData();

            // If the body is dynamic, pull it towards our location
            if (actor_body && body != actor_body && actor_body->GetType() == b2_dynamicBody) {
                auto space = world->world_size;
                auto direction = body->GetWorldCenter () - actor_body->GetWorldCenter ();

                // Wrap the force around the vertical borders (left & right)
                if (direction.x > 0.5 * space.x) {
                    direction.x -= space.x;
                } else if (direction.x < -0.5 * space.x) {
                    direction.x += space.x;
                }
                // Wrap the force around the horizontal borders (top & bottom)
                if (direction.y > 0.5 * space.y) {
                    direction.y -= space.y;
                } else if (direction.y < -0.5 * space.y) {
                    direction.y += space.y;
                }

                // Calculate the gravitational pull based on distance and apply it onto the body
                auto force = mass * PLANET_GRAVITY * updateState.dt / direction.Normalize ();
                actor_body->ApplyLinearImpulseToCenter (force * direction, PLANET_WAKES_SHIP);
            }

            // Update the iterator
            current_actor = current_actor->getNextSibling();
        }

        Actor::update (updateState);
    }

    Planet::~Planet () noexcept {
        detach();
    }
}

/* Copyright © 2022 Aaron Alef */
