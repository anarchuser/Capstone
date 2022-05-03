#ifndef CAPSTONE_REMOTESPACESHIP_H
#define CAPSTONE_REMOTESPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Spaceship.h"

#include "Server/Server.h"
#include "Server/Callback/DirectionCallback.h"

#include <functional>

#define REMOTE_SPACESHIP_COLOR {255, 0, 0}

namespace kt {
    using namespace oxygine;

    class RemoteSpaceship: public Spaceship {
    public:
        RemoteSpaceship (World & world, Resources & res, Vector2 const & pos, float scale = 1);

        /// Replace current directions with new ones
        void updateDirection (Direction new_dir);

        [[nodiscard]] cg::DirectionCallback getCallback();
    };

} // kt

#endif //CAPSTONE_REMOTESPACESHIP_H

/* Copyright © 2022 Aaron Alef */
