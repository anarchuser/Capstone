#include "World.h"

namespace kt {
    World::World (ResAnim * background, b2Vec2 size)
            : world_size {size}
            {
        setName ("World");
        setResAnim (background);
    
        // Size and position of the World Sprite
        setSize (getStage()->getSize());
        setPosition (getStage()->getPosition());
        
        OX_ASSERT(world_size.x > 0);
        OX_ASSERT(world_size.y > 0);

        // Set the collision handler converting box2d collisions to corresponding oxygine events
        world.SetContactListener (new CollisionListener);

        // Listen for ` events to toggle box2d debug view
        getStage()->addEventListener (ox::KeyEvent::KEY_DOWN, [this](Event * event) {
            auto key = safeCast<KeyEvent *> (event)->data->keysym.scancode;
            switch (key) {
                case SDL_SCANCODE_GRAVE:
                    event->stopImmediatePropagation();
                    toggleDebugDraw ();
                    break;
            }
        });
    }

    void World::update (UpdateState const & updateState) {
        // Update all children
        Actor::update (updateState);

        // Advance physical world depending on time passed
        world.Step (1e-3f * updateState.dt, 1, 1);

        // box2d's way of iterating through all physical bodies
        b2Body * current_body = world.GetBodyList();
        // Schedule bodies to get destroyed to avoid iterator invalidation
        std::vector <b2Body *> to_delete;

        while (current_body) {
            // Try to retrieve the oxygine actor connected to the current body
            auto * current_actor = (Actor *) current_body->GetUserData().pointer;

            // If it exists update the actor's position to match the body's
            if (current_actor) {
                // Ensure no object is out of bounds
                auto pos = wrap (current_body->GetPosition());
                auto angle = current_body->GetAngle();
                current_body->SetTransform (pos, angle);
                current_actor->setPosition (convert (pos));
                current_actor->setRotation (angle);
            } else {
                // If no actor is connected schedule its destruction
                to_delete.push_back (current_body);
            }

            // Update iterator
            current_body = current_body->GetNext();
        }

        // Once all valid actors are updated, delete all dangling bodies
        for (auto * garbage : to_delete) {
            world.DestroyBody (garbage);
        }
    }

    b2Vec2 World::wrap (b2Vec2 pos) const {
        // Wrap the given coordinates to be within world boundaries
        pos.x = fmod (std::ceil ((long double) std::abs (pos.x) / world_size.x) * world_size.x + pos.x, world_size.x);
        pos.y = fmod (std::ceil ((long double) std::abs (pos.y) / world_size.y) * world_size.y + pos.y, world_size.y);

        return pos;
    }

    b2Vec2 World::convert (Vector2 const & pos) const {
        // Cache the ratio graphical world / physical world
        static long double scale_x = getSize().x / world_size.x;
        static long double scale_y = getSize().y / world_size.y;
        return { float (pos.x / scale_x)
               , float (pos.y / scale_y)
        };
    }

    Vector2 World::convert (b2Vec2 const & pos) const {
        // Cache the ratio physical world / graphical world
        static long double scale_x = world_size.x / getSize().x;
        static long double scale_y = world_size.y / getSize().y;
        return { float (pos.x / scale_x)
               , float (pos.y / scale_y)
        };
    }

    void World::toggleDebugDraw () {
        // If a debug draw exists, remove it
        if (debugDraw) {
            logs::messageln ("disable debug draw");
            debugDraw->detach();
            debugDraw = nullptr;
            return;
        }

        // If no debug draw exists yet create a new one
        logs::messageln ("enable debug draw");
        debugDraw = new Box2DDraw;
        debugDraw->SetFlags (b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
        debugDraw->attachTo (this);
        debugDraw->setWorld (getSize().x / world_size.x, & world);
        debugDraw->setPriority (1);
    }

    void World::addChild (spActor child) {
        if (!child) return;

        auto * body = (b2Body *) child->getUserData();
        if (!body) return;

        // Update child position to be within world boundaries
        body->SetTransform (wrap (body->GetPosition()), body->GetAngle());
        child->setPosition (convert (body->GetPosition()));

        // Forward call to parent class
        Actor::addChild (child);
    }
}

/* Copyright © 2022 Aaron Alef */
