#include "World.h"

namespace kt {
    World::World (ResAnim * background, b2Vec2 size): world (b2Vec2_zero), world_size {size} {
        setName ("World");
        setResAnim (background);
    
        // Size and position of the World Sprite
        setSize (getStage()->getSize());
        setPosition (getStage()->getPosition());
        
        OX_ASSERT(world_size.x > 0);
        OX_ASSERT(world_size.y > 0);

        world.SetContactListener (new CollisionListener);

        getStage()->addEventListener (ox::KeyEvent::KEY_DOWN, [=](Event * event) {
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
        world.Step (1/ FPS, 1, 1);

        b2Body * current_body = world.GetBodyList();
        std::vector <b2Body *> to_delete;

        while (current_body) {
            auto * current_actor = (Actor *) current_body->GetUserData().pointer;

            if (current_actor) {
                // Ensure no actor is out of bounds
                auto pos = wrap (current_body->GetPosition());
                auto angle = current_body->GetAngle();
                current_body->SetTransform (pos, angle);
                current_actor->setPosition (convert (pos));
                current_actor->setRotation (angle);
            } else {
                // Body without actor -> remove
                to_delete.push_back (current_body);
            }

            current_body = current_body->GetNext();
        }

        for (auto * garbage : to_delete) {
            world.DestroyBody (garbage);
        }

        Actor::update (updateState);
    }

    b2Vec2 World::wrap (b2Vec2 pos) const {
        pos.x = fmod (std::ceil ((long double) std::abs (pos.x) / world_size.x) * world_size.x + pos.x, world_size.x);
        pos.y = fmod (std::ceil ((long double) std::abs (pos.y) / world_size.y) * world_size.y + pos.y, world_size.y);

        return pos;
    }

    Vector2 World::wrap (Vector2 pos) const {
        return convert (wrap (convert (pos)));
    }

    b2Vec2 World::convert (Vector2 const & pos) const {
        static float scale_x = getSize().x / world_size.x;
        static float scale_y = getSize().y / world_size.y;
        return {
            pos.x / scale_x,
            pos.y / scale_y
        };
    }

    Vector2 World::convert (b2Vec2 const & pos) const {
        static float scale_x = world_size.x / getSize().x;
        static float scale_y = world_size.y / getSize().y;
        return {
            pos.x / scale_x,
            pos.y / scale_y
        };
    }

    void World::toggleDebugDraw () {
        if (debugDraw) {
            logs::messageln ("disable debug draw");

            debugDraw->detach();
            debugDraw = nullptr;
            return;
        }

        logs::messageln ("enable debug draw");

        debugDraw = new Box2DDraw;
        //TODO: put flags somewhere else
        debugDraw->SetFlags (b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
        debugDraw->attachTo (this);
        debugDraw->setWorld (getSize().x / world_size.x, & world);
        debugDraw->setPriority (1);
    }

    void World::addChild (spActor child) {
        auto * body = (b2Body *) child->getUserData();
        if (body) {
            body->SetTransform (wrap (body->GetPosition()), body->GetAngle());
            child->setPosition (convert (body->GetPosition()));
        }

        Actor::addChild (child);
    }
}

/* Copyright © 2022 Aaron Alef */
