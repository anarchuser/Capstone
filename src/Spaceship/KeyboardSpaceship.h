#ifndef CAPSTONE_KB_SPACESHIP_H
#define CAPSTONE_KB_SPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Spaceship.h"
#include "World/World.h"

#include "Data/Spaceship.h"

/// Distinct colour to separate the user-controlled spaceship from all the other ones
#define KEYBOARD_SPACESHIP_COLOR {0, 0, 255}

namespace kt {
    using namespace oxygine;

    /// Spaceship instance controllable using WASD or arrow keys
    class KeyboardSpaceship: public Spaceship {
    private:
        /// The directions queried by the user
        cg::Direction queried;

        /// Client scope needed to wait on callback promises
        kj::WaitScope & waitscope;

        /// When issuing commands, update Spaceship-specific flags
        void onSteeringEvent (ox::KeyEvent * event);

    public:
        /// Creates a new human-controllable spaceship. Only one such ship is supposed to exist in a game
        KeyboardSpaceship (World & world, Resources * res, std::string const & username, kj::WaitScope & waitscope);

        /// Send directions queried by the user to the callback
        void update (UpdateState const & updateState) override;
    };
    DECLARE_SMART(KeyboardSpaceship, spKeyboardSpaceship);
}

#endif //CAPSTONE_KB_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
