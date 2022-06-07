#include "Game.h"

namespace kt {

    bool Game::initialised = false;

    void Game::init () {
        // Ensure `init` is only run once
        static bool ran = false;
        if (ran) {
            logs::warning ("Tried to reinitialise application");
            return;
        }
        ran = true;

        // Initialise window settings
        core::init_desc desc;
        desc.title = WINDOW_TITLE;
        desc.w = WINDOW_WIDTH;
        desc.h = WINDOW_HEIGHT;

        // Initialise oxygine with window settings above
        core::init (& desc);

        // Instantiate Stage as root node for the application. Globally accessible
        Stage::instance = new Stage();
        Point size = core::getDisplaySize();
        getStage()->setSize(size);

        // Initialisation done
        initialised = true;
    }

    void Game::run () {
        // Ensure `run` is only run once
        static bool ran = false;
        if (ran) {
            logs::warning ("Tried to start a game that has already been started");
            return;
        }
        // Ensure game has been initialised already
        if (!initialised) {
            logs::messageln ("Called run before initialisation. Initialising now");
            init ();
        }
        ran = true;

        // Guard to detect memory leaks
        ObjectBase::__startTracingLeaks();

#ifdef DEBUG
        logs::messageln ("Debug view enabled");
        DebugActor::show();
#endif

        // Create main menu scene
        new MenuScene ();
        logs::messageln ("Scene initialisation complete");

        // Main game loop. Returns true if done
        int done;
        do {
            done = loop ();
        } while (!done);

        // Release all internal components and the stage
        core::release();

        // Print any still remaining object. For debugging memory leaks
        ObjectBase::dumpCreatedObjects();

        // End of memory leak guard
        ObjectBase::__stopTracingLeaks();
    }

    bool Game::loop () {
        // Update engine-internal components
        // If input events are available, they are passed to Stage::instance.handleEvent
        // If the function returns true, it means that the user requested the application to terminate
        bool done = core::update();

        // Update all actors. Actor::update will also be called for all its children
        getStage()->update();

        if (core::beginRendering())
        {
            // Base background colour
            Color clearColor = CLEAR_COLOUR;
            Rect viewport(Point(0, 0), core::getDisplaySize());
            // Render all actors inside the stage. Actor::render will also be called for all its children
            getStage()->render(clearColor, viewport);

            core::swapDisplayBuffers();
        }

        return done;
    }
}

/* Copyright © 2022 Aaron Alef */
