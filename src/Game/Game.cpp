#include "Game.h"

namespace kt {

    bool Game::initialised = false;

    void Game::init () {
        // Ensure `init` is only run once
        static bool ran = false;
        if (ran) {
            LOG (WARNING) << "Tried to reinitialise application";
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

        initialised = true;
    }

    void Game::run () {
        // Ensure `run` is only run once
        static bool ran = false;
        if (ran) {
            LOG (WARNING) << "Tried to start a game that has already been started";
            return;
        }
        // Ensure game has been initialised already
        while (!initialised) {
            LOG (INFO) << "Trying to start game before initialisation. Initialising now";
            init ();
        }
        ran = true;

        // Guard to detect memory leaks
        ObjectBase::__startTracingLeaks();

#ifdef DEBUG
        logs::messageln ("Debug view enabled");
        LOG (INFO) << "Debug view enabled";
        DebugActor::show();
#endif

        getStage()->addEventListener (KeyEvent::KEY_DOWN, [=] (Event const * event) {
            auto const * _event = safeCast <KeyEvent const *> (event);

            switch (_event->data->keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    onRequestExit(_event);
                    break;
            }
        });

        // TODO: start menu here
        spDialog dialog = new Dialog ({0, 0}, getStage()->getSize(), "Menu");
        dialog->addButton ("New Game", [](Event const *) {
            getStage()->removeChildren();

            // Create main actor governing the game
            spMainActor mainActor = new MainActor (RANDOM_SEED);
            getStage()->addChild (mainActor);
        });
        dialog->addButton ("Hello World", [](Event const *) { logs::messageln ("Hello World");});
        dialog->addButton ("Exit", onRequestExit);
        getStage()->addChild (dialog);

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

        // Update our stage
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

    void Game::onRequestExit (Event const * event) {
        static bool active = false;
        static auto size = getStage()->getSize();
        static spDialog dialog = [=] () {
            auto dialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 5}, "Do you want to quit the game?");
            // TODO: add options to disconnect or return to main menu
            dialog->addButton ("Quit", [] (Event const *) { core::requestQuit (); });
            return dialog;
        }();

        if (!active) {
            getStage()->addChild (dialog);
            active = true;
        } else {
            getStage()->removeChild (dialog);
            active = false;
        }
    }
}

/* Copyright © 2022 Aaron Alef */
