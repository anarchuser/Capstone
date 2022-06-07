#ifndef CAPSTONE_REMOTE_SPACESHIP_H
#define CAPSTONE_REMOTE_SPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Spaceship.h"

/// Distinct colour flavour to distinguish remote ships from the keyboard-controlled one
#define REMOTE_SPACESHIP_COLOR {255, 0, 0}

namespace kt {
    using namespace oxygine;

    /// Spaceship specialisation controlled by a remote counterpart only
    class RemoteSpaceship: public Spaceship {
    public:
        /// Construct a new remote ship with given name
        RemoteSpaceship (World & world, Resources * res, std::string const & username);
    };
    DECLARE_SMART (RemoteSpaceship, spRemoteSpaceship);

} // kt

#endif //CAPSTONE_REMOTE_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
