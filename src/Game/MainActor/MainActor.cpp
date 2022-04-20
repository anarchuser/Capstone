#include "MainActor.h"

namespace kt {

    MainActor::MainActor () {
        // Load all required game assets
        gameResources.loadXML (GAME_RESOURCES);
    }

    MainActor::~MainActor () noexcept {
        // Free all game assets
        gameResources.free();
    }
}

/* Copyright © 2022 Aaron Alef */
