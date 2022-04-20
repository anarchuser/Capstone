#ifndef CAPSTONE_GAME_H
#define CAPSTONE_GAME_H

#include "config.h"
#include "game_config.h"

#include "oxygine-framework.h"

namespace kt {

    using namespace oxygine;

    /// Provides methods for setting up oxygine and starting a game client
    class Game {
    public:
        /// Initialise oxygine (set window properties, create root node)
        static void init ();

        /// Call this to start running the game
        static void run ();

    private:
        /// False until initialised
        static bool initialised;

        /// Function called for each frame to update game
        static bool loop ();
    };
}

#endif //CAPSTONE_GAME_H

/* Copyright © 2022 Aaron Alef */
