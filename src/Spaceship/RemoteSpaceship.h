#ifndef CAPSTONE_REMOTESPACESHIP_H
#define CAPSTONE_REMOTESPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Spaceship.h"

#define REMOTE_SPACESHIP_COLOR {255, 0, 0}

namespace kt {
    using namespace oxygine;

    class RemoteSpaceship: public Spaceship {
    public:
        RemoteSpaceship (World & world, Resources * res, std::string const & username);

        void destroy () override;
    };
    DECLARE_SMART (RemoteSpaceship, spRemoteSpaceship);

} // kt

#endif //CAPSTONE_REMOTESPACESHIP_H

/* Copyright © 2022 Aaron Alef */
