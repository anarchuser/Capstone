#ifndef CAPSTONE_MAINACTOR_H
#define CAPSTONE_MAINACTOR_H

#include "config.h"
#include "random/random.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Game/Planet/Planet.h"
#include "Game/Spaceship/KeyboardSpaceship.h"
#include "Game/World/World.h"

/// Path to all resources used in the game
#define GAME_RESOURCES  (PROJECT_ROOT "/src/Game/data/res.xml")

/// [OPTIONAL] The random seed determines the placement of planets, amongst others
#define RANDOM_SEED

#define FPS 60.0

namespace kt {
    using namespace oxygine;

    class MainActor : public Actor {
    private:
        Resources gameResources;
        HashedRNG rng;

    public:
        MainActor ();
        explicit MainActor (std::size_t seed);
        ~MainActor() noexcept override;

        void quitGame();
    };

    DECLARE_SMART(MainActor, spMainActor);
}

#endif //CAPSTONE_MAINACTOR_H

/* Copyright © 2022 Aaron Alef */
