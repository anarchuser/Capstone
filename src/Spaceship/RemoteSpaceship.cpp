#include "RemoteSpaceship.h"

namespace kt {
    // TODO: ping to one specific server instance?
    RemoteSpaceship::RemoteSpaceship (World & world, Resources * res, std::string const & username)
            : Spaceship (world, res, username)
            {
                setAddColor (REMOTE_SPACESHIP_COLOR);
            }

    void RemoteSpaceship::destroy () {
        Spaceship::destroy();
        updateScoreboard ("ghost");
    }
}

/* Copyright © 2022 Aaron Alef */
