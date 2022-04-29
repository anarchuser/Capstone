#include "RemoteSpaceship.h"

namespace kt {
    // TODO: connect to one specific server instance?
    RemoteSpaceship::RemoteSpaceship (World & world, Resources & res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale) {

        cg::SynchroImpl::updateDirectionCallback = [this] (int w, int s, int a, int d) {
            ONCE (setAwake (true));
            if (w >= 0) accelerate  = w;
            if (s >= 0) decelerate  = s;
            if (a >= 0) rotateLeft  = a;
            if (d >= 0) rotateRight = d;
        };
    }
}

/* Copyright © 2022 Aaron Alef */
