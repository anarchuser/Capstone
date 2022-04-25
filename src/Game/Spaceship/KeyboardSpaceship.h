#ifndef CAPSTONE_KB_SPACESHIP_H
#define CAPSTONE_KB_SPACESHIP_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Game/World/World.h"
#include "Game/Spaceship/Spaceship.h"

namespace kt {
    using namespace oxygine;

    DECLARE_SMART(KeyboardSpaceship, spKeyboardSpaceship);

    class KeyboardSpaceship: public Spaceship {
    public:
        KeyboardSpaceship (World & world, oxygine::ResAnim * animation, Vector2 const & pos, float scale = 1);

        void onSteeringEvent (ox::KeyEvent * event);

        static spKeyboardSpaceship instance;
    };

}

#endif //CAPSTONE_KB_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
