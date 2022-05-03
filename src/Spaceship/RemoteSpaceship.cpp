#include "RemoteSpaceship.h"

namespace kt {
    // TODO: connect to one specific server instance?
    RemoteSpaceship::RemoteSpaceship (World & world, Resources & res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale)
    {
        setAddColor (REMOTE_SPACESHIP_COLOR);

        setAwake (true);
    }

    void RemoteSpaceship::updateDirection (Direction new_dir) {
        direction = new_dir;
    };

    cg::DirectionCallback RemoteSpaceship::getCallback() {
        return {
            [this] (Direction dir) { updateDirection (dir); },
            [this] () { destroy(); },
        };
    }

    void RemoteSpaceship::destroy () {
        Spaceship::destroy();
        updateScoreboard ("ghost");
    }
}

/* Copyright © 2022 Aaron Alef */
