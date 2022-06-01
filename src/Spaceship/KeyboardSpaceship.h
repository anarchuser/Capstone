#ifndef CAPSTONE_KB_SPACESHIP_H
#define CAPSTONE_KB_SPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Spaceship.h"
#include "World/World.h"

#include "Data/Spaceship.h"

#define KEYBOARD_SPACESHIP_COLOR {0, 0, 255}

namespace kt {
    using namespace oxygine;

    /// Spaceship instance controllable using WASD or arrow keys
    class KeyboardSpaceship: public Spaceship {
    private:
        cg::SendItemCallback onUpdate;
        cg::DoneCallback onDone;

        cg::Direction queried;

    public:
        /// Creates a new human-controllable spaceship. Only one such ship may exist in a game
        KeyboardSpaceship (World & world, Resources * res, std::string const & username);
        ~KeyboardSpaceship() noexcept override = default;

        /// When issuing commands, update Spaceship-specific flags
        void onSteeringEvent (ox::KeyEvent * event);

        void setOnUpdate (cg::SendItemCallback && onUpdate);
        void update (UpdateState const & us) override;

        void setOnDone (cg::DoneCallback && onDone);
        void destroy () override;

        kj::Own <cg::ShipSinkImpl> getSink() override;
    };

    DECLARE_SMART(KeyboardSpaceship, spKeyboardSpaceship);
}

#endif //CAPSTONE_KB_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
