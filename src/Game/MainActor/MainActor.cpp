#include "MainActor.h"

namespace kt {
    MainActor::MainActor (): MainActor (generateSeed()) {}
    MainActor::MainActor (std::size_t seed): rng (seed) {
        logs::messageln ("Seed: %lu", seed);

        // Load all required game assets
        gameResources.loadXML (GAME_RESOURCES);

        spWorld world = new World (WORLD_SIZE);
        addChild (world);
    }

    MainActor::~MainActor () noexcept {
        // Free all game assets
        gameResources.free();
    }
}

/* Copyright © 2022 Aaron Alef */
