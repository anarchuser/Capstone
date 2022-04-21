#include "MainActor.h"

namespace kt {
    MainActor::MainActor (): MainActor (generateSeed()) {}
    MainActor::MainActor (std::size_t seed): rng (seed) {
        logs::messageln ("Seed: %lu", seed);

        // Load all required game assets
        gameResources.loadXML (GAME_RESOURCES);

        spWorld world = new World (WORLD_SIZE);
        addChild (world);

        // Generate a couple of planets, number based on world size
        // TODO: randomise planet size
        auto resource = gameResources.getResAnim ("venus");
        for (std::size_t i = 0; i < PLANETS_PER_PIXEL * world->world_size.x * world->world_size.y; i++) {
            auto planet = new Planet (* world, resource, {
                        float (rng.random ({100, world->getSize().x - 100})),
                        float (rng.random ({100, world->getSize().y - 100}))
                }, 0.5);
            world->addChild (planet);
        }

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
