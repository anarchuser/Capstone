#include "World.h"

namespace kt {
    World::World (b2Vec2 size): world (b2Vec2_zero), world_size {size} {
        // Size of the World Sprite
        setSize (getStage()->getSize());
        scale = { getSize().x / world_size.x,
                  getSize().y / world_size.y};

        logs::messageln ("Graphical size: %f / %f", getSize().x, getSize().y);
        logs::messageln ("Physical  size: %f / %f", world_size.x, world_size.y);
        logs::messageln ("Gra/Phy  scale: %f / %f", scale.x, scale.y);

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
        while (pos.x < 0) pos.x += world_size.x;
        while (pos.y < 0) pos.y += world_size.y;
        pos.x = fmod (pos.x, world_size.x);
        pos.y = fmod (pos.y, world_size.y);

        return pos;
    }

    b2Vec2 World::convert (Vector2 const & pos) const {
        return {
            pos.x / scale.x,
            pos.y / scale.y
        };
    }

    Vector2 World::convert (b2Vec2 const & pos) const {
        return {
            pos.x * scale.x,
            pos.y * scale.y
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
