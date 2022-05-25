#ifndef CAPSTONE_GAMESCENE_H
#define CAPSTONE_GAMESCENE_H

#include "config.h"
#include "Network/config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include <thread>
#include <memory>

#include "src/UI/Dialog.h"
#include "src/Planet/Planet.h"
#include "src/Spaceship/RemoteSpaceship.h"
#include "src/World/World.h"
#include "Scene.h"
#include "MenuScene.h"
#include "Backend/Backend.h"

#include "Random/random.h"
#include "Network/Backend.h"
#include "Network/Registrar/Registrar.h"

/// [OPTIONAL] The random seed determines the placement of planets, amongst others
#define RANDOM_SEED

namespace kt {
    using namespace oxygine;

    /// Handles the actual game. Responsible for initialising the World with its objects and providing an options menu
    class GameScene : public Scene {
    private:
        Backend backend;
        std::vector <std::unique_ptr <capnp::EzRpcClient>> remoteClients;
        capnp::EzRpcClient client;
        kj::WaitScope & waitscope;

        struct Handle {
            ::Backend::Registrar::Client registrar;
            ::Backend::Synchro::Client synchro;
        } handle;

        /// Resources used throughout the game (font, background, sprites)
        Resources gameResources;
        /// Random number generator based on the seed given. Ensures every random number used depends on this seed
        HashedRNG rng;

        static std::size_t requestSeed (std::string const & ip, unsigned short port) ;
        void joinGame (std::string const & ip, unsigned short port);

        kj::Own <cg::RegistrarImpl> getRegistrarImpl ();
        
    public:
        /// Inject a new Game instance with random seed into the stage
        GameScene ();
        /// Directly ping to an existing Game remotely
        GameScene (std::string const & ip, unsigned short port);
        /// Inject a new Game instance with given seed into the stage
        explicit GameScene (std::size_t seed);
        ~GameScene() noexcept override;

        /// Updates its children, given that the game is not paused (i.e., hard and soft pause set to false)
        void update(UpdateState const & us) override;

        /// Toggle menu. Opens dialog with options to start a new game with same or new seed, or to quit the game
        void onMenu (Event * event);
        /// Restart game with previous seed
        void onRestart (Event * event);
        /// Start game with new, random seed
        void onNewGame (Event * event);
        /// Quit game and return to main menu
        void onDisconnect (Event * event);
        /// Quit game and close application
        void onQuit (Event * event);
    };

    DECLARE_SMART(GameScene, spGameScene);
}

#endif //CAPSTONE_GAMESCENE_H

/* Copyright © 2022 Aaron Alef */
