#ifndef CAPSTONE_GAME_H
#define CAPSTONE_GAME_H

#include "config.h"

#include "oxygine-framework.h"

#include "src/UI/Dialog.h"
#include "src/Scene/MenuScene.h"

/// Height of game client, in pixels
#define WINDOW_HEIGHT   1000
/// Width of game clients, in pixels
#define WINDOW_WIDTH    1000

/// Application title
#define WINDOW_TITLE    "Kepler's Torus"

/// Default "fallback" application background colour. Overridden by `sky` and `dialog` images, respectively
#define CLEAR_COLOUR    {30, 30, 30, 255};

namespace kt {
    using namespace oxygine;

    /// Set of static methods to initialise oxygine and start the game
    class Game final {
    public:
        Game() = delete;

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
