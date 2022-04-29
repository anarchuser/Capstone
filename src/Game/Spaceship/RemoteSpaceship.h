#ifndef CAPSTONE_REMOTESPACESHIP_H
#define CAPSTONE_REMOTESPACESHIP_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Spaceship.h"

#include "Server/Server.h"

namespace kt {
    using namespace oxygine;

    class RemoteSpaceship: public Spaceship {
    public:
        RemoteSpaceship (World & world, Resources & res, Vector2 const & pos, float scale = 1);

    };

} // kt

#endif //CAPSTONE_REMOTESPACESHIP_H

/* Copyright © 2022 Aaron Alef */
