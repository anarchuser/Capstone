#ifndef CAPSTONE_GAMESCENE_H
#define CAPSTONE_GAMESCENE_H

#include "config.h"
#include "Game/config.h"
#include "random/random.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Game/UI/Dialog.h"
#include "Game/Planet/Planet.h"
#include "Game/Spaceship/KeyboardSpaceship.h"
#include "Game/World/World.h"
#include "Game/Scene/Scene.h"
//#include "Game/Scene/config.h"
#include "MenuScene.h"

/// [OPTIONAL] The random seed determines the placement of planets, amongst others
#define RANDOM_SEED

#define FPS 60.0

namespace kt {
    using namespace oxygine;

    class GameScene : public Scene {
    private:
        Resources gameResources;
        HashedRNG rng;
        bool hardPause = false;
        bool softPause = false;

    public:
        GameScene ();
        explicit GameScene (std::size_t seed);
        ~GameScene() noexcept override;

        void update(UpdateState const & us) override;

        void onMenu (Event * event);
        void onRestart (Event * event);
        void onNewGame (Event * event);
        void onAbandon (Event * event);
    };

    DECLARE_SMART(GameScene, spGameScene);
}

#endif //CAPSTONE_GAMESCENE_H

/* Copyright © 2022 Aaron Alef */
