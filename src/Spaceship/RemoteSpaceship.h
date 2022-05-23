#ifndef CAPSTONE_REMOTESPACESHIP_H
#define CAPSTONE_REMOTESPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Spaceship.h"

#include "Network/Backend.h"
#include "Network/ShipHandle/ShipHandle.h"

#include <functional>

#define REMOTE_SPACESHIP_COLOR {255, 0, 0}

namespace kt {
    using namespace oxygine;

    class RemoteSpaceship: public Spaceship {
    public:
        RemoteSpaceship (World & world, Resources * res);

        void destroy () override;

        kj::Own <cg::ShipHandleImpl> getHandle () override;
    };
    DECLARE_SMART (RemoteSpaceship, spRemoteSpaceship);

} // kt

#endif //CAPSTONE_REMOTESPACESHIP_H

/* Copyright © 2022 Aaron Alef */
