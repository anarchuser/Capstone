#include "RemoteSpaceship.h"

namespace kt {
    RemoteSpaceship::RemoteSpaceship (World & world, Resources * res, std::string const & username)
            : Spaceship (world, res, username)
            {
                setAddColor (REMOTE_SPACESHIP_COLOR);
            }
}

/* Copyright © 2022 Aaron Alef */
