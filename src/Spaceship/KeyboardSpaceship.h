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
        /// Callback to send directions & state to the backend
        cg::SendItemCallback onUpdate;
        /// Callback to tell the backend that we died
        cg::DoneCallback onDone;

        /// The directions queried by the user
        cg::Direction queried;

    public:
        /// Creates a new human-controllable spaceship. Only one such ship may exist in a game
        KeyboardSpaceship (World & world, Resources * res, std::string const & username);

        /// When issuing commands, update Spaceship-specific flags
        void onSteeringEvent (ox::KeyEvent * event);

        /// Update how to behave on update
        void setOnUpdate (cg::SendItemCallback && onUpdate);
        /// Send directions queried by the user to the callback
        void update (UpdateState const & us) override;

        /// Update the callback for when we crash
        void setOnDone (cg::DoneCallback && onDone);
        /// Tell the backend we died before we destroy ourselves
        void destroy () override;

        /// Override the ShipSink implementation to ignore onDone events - we caused them originally anyways
        kj::Own <cg::ShipSinkImpl> getSink() override;
    };
    DECLARE_SMART(KeyboardSpaceship, spKeyboardSpaceship);
}

#endif //CAPSTONE_KB_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
