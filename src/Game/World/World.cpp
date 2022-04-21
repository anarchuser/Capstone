#include "World.h"

namespace kt {
    World::World (b2Vec2 size): world (b2Vec2_zero), world_size {size} {
        // Size of the World Sprite
        setSize (getStage()->getSize());

        OX_ASSERT(world_size.x > 0);
        OX_ASSERT(world_size.y > 0);

        ox::getStage()->addEventListener (ox::KeyEvent::KEY_DOWN, [=](Event * event) {
            auto key = safeCast<KeyEvent *> (event)->data->keysym.scancode;
            switch (key) {
                case SDL_SCANCODE_GRAVE:
                    toggleDebugDraw ();
                    break;
            }
        });
    }

    b2Vec2 World::wrap (b2Vec2 pos) const {
        pos.x = fmod (std::ceil ((long double) pos.x + std::abs (pos.x) / world_size.x) * world_size.x + pos.x, world_size.x);
        pos.y = fmod (std::ceil ((long double) pos.y + std::abs (pos.y) / world_size.y) * world_size.y + pos.y, world_size.y);

        return pos;
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

    void kt::World::toggleDebugDraw () {
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
}

/* Copyright © 2022 Aaron Alef */
