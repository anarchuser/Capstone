#include "RemoteSpaceship.h"

namespace kt {
    // TODO: connect to one specific server instance?
    RemoteSpaceship::RemoteSpaceship (World & world, Resources & res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale) {}
}

/* Copyright © 2022 Aaron Alef */
