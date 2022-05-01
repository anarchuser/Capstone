#include "RemoteSpaceship.h"

namespace kt {
    // TODO: connect to one specific server instance?
    RemoteSpaceship::RemoteSpaceship (World & world, Resources & res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale) {

        cg::SynchroImpl::updateDirectionCallback = [this] (Direction new_dir) {
            ONCE (setAwake (true));
            if (new_dir.accelerate >= 0) direction.accelerate  = new_dir.accelerate;
            if (new_dir.decelerate >= 0) direction.decelerate  = new_dir.decelerate;
            if (new_dir.rotateLeft >= 0) direction.rotateLeft  = new_dir.rotateLeft;
            if (new_dir.rotateRight>= 0) direction.rotateRight = new_dir.rotateRight;
        };
    }
}

/* Copyright © 2022 Aaron Alef */
