#ifndef CAPSTONE_GAME_H
#define CAPSTONE_GAME_H

#include "config.h"

#include "oxygine-framework.h"

#include "Game/MainActor/MainActor.h"
#include "Game/UI/Dialog.h"

/// Height of game client, in pixels
#define WINDOW_HEIGHT   1080
/// Width of game clients, in pixels
#define WINDOW_WIDTH    1080

/// Application title
#define WINDOW_TITLE    "Kepler's Torus"

/// Application background "space" colour (until a proper background exists)
#define CLEAR_COLOUR    {30, 30, 30, 255};

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
