#ifndef CAPSTONE_KB_SPACESHIP_H
#define CAPSTONE_KB_SPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "src/World/World.h"
#include "Spaceship.h"

#include "Server/Server.h"

#include <thread>

#define KEYBOARD_SPACESHIP_COLOR {0, 0, 255}

namespace kt {
    using namespace oxygine;

    DECLARE_SMART(KeyboardSpaceship, spKeyboardSpaceship);

    /// Spaceship instance controllable using WASD or arrow keys
    class KeyboardSpaceship: public Spaceship {
    private:
        capnp::EzRpcClient client;
        Synchro::ItemSink::Client sink;

    public:
        /// Creates a new human-controllable spaceship. Only one such ship may exist in a game
        KeyboardSpaceship (World & world, Resources & res, Vector2 const & pos, float scale = 1);
        ~KeyboardSpaceship() noexcept override;

        /// When issuing commands, update Spaceship-specific flags
        void onSteeringEvent (ox::KeyEvent * event);

        void update (UpdateState const & us) override;

        void destroy () override;

        /// Current keyboard controlled spaceship instance
        static KeyboardSpaceship * instance;
    };

}

#endif //CAPSTONE_KB_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
