#include "RemoteSpaceship.h"

namespace kt {
    // TODO: connect to one specific server instance?
    RemoteSpaceship::RemoteSpaceship (World & world, Resources & res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale) {

        cg::SynchroImpl::updateDirectionCallback = [this] (bool w, bool s, bool a, bool d) {
            ONCE (setAwake (true));
            accelerate  ^= w;
            decelerate  ^= s;
            rotateLeft  ^= a;
            rotateRight ^= d;
        };
    }
}

/* Copyright © 2022 Aaron Alef */
