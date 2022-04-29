#ifndef CAPSTONE_KB_SPACESHIP_H
#define CAPSTONE_KB_SPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Game/World/World.h"
#include "Game/Spaceship/Spaceship.h"

namespace kt {
    using namespace oxygine;

    DECLARE_SMART(KeyboardSpaceship, spKeyboardSpaceship);

    /// Spaceship instance controllable using WASD or arrow keys
    class KeyboardSpaceship: public Spaceship {
    public:
        /// Creates a new human-controllable spaceship. Only one such ship may exist in a game
        KeyboardSpaceship (World & world, Resources & res, Vector2 const & pos, float scale = 1);

        /// When issuing commands, update Spaceship-specific flags
        void onSteeringEvent (ox::KeyEvent * event);

        /// Current keyboard controlled spaceship instance
        static spKeyboardSpaceship instance;
    };

}

#endif //CAPSTONE_KB_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
