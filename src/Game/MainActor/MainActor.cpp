#include "MainActor.h"

namespace kt {
    MainActor::MainActor (): MainActor (generateSeed()) {}
    MainActor::MainActor (std::size_t seed): rng (seed) {
        logs::messageln ("Seed: %lu", seed);

        // Load all required game assets
        gameResources.loadXML (GAME_RESOURCES);

        spWorld world = new World (WORLD_SIZE);
        addChild (world);

        getStage()->addEventListener (KeyEvent::KEY_DOWN, [=] (Event const * event) {
            auto const * _event = safeCast <KeyEvent const *> (event);

            switch (_event->data->keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    quitGame();
                    break;
            }
        });
    }

    MainActor::~MainActor () noexcept {
        // Free all game assets
        gameResources.free();
    }

    void MainActor::quitGame () {
        // TODO: add quit / options dialog
        core::requestQuit();
    }
}

/* Copyright © 2022 Aaron Alef */
