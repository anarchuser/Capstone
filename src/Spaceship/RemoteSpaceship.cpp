#include "RemoteSpaceship.h"

namespace kt {
    // TODO: ping to one specific server instance?
    RemoteSpaceship::RemoteSpaceship (World & world, Resources & res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale)
    {
        setAddColor (REMOTE_SPACESHIP_COLOR);

        setAwake (true);

        updateScoreboard();
    }

    void RemoteSpaceship::destroy () {
        Spaceship::destroy();
        updateScoreboard ("ghost");
    }
}

/* Copyright © 2022 Aaron Alef */
