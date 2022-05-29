#ifndef CAPSTONE_GAMESCENE_H
#define CAPSTONE_GAMESCENE_H

#include "config.h"
#include "Network/config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include <thread>
#include <memory>

#include "UI/Dialog.h"
#include "Planet/Planet.h"
#include "Spaceship/RemoteSpaceship.h"
#include "Spaceship/KeyboardSpaceship.h"
#include "World/World.h"
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
        /// Resources used throughout the game (font, background, sprites)
        Resources gameResources;
        /// Random number generator based on the seed given. Ensures every random number used depends on this seed
        HashedRNG rng;

        /* All objects related to the connection to the backend */
        Backend backend;
        std::unordered_map <std::string, kj::Own <capnp::EzRpcClient>> remoteClients;
        capnp::EzRpcClient client;
        kj::WaitScope & waitscope;

        struct Handle {
            cg::Registrar_t registrar;
            cg::Synchro_t synchro;
            std::unique_ptr <cg::ShipSink_t> keyboard_sink = nullptr;
        } handle;

        /* References to all objects stored in the world */
        struct Actors {
            spWorld world;
            spKeyboardSpaceship localShip;
            std::vector <spRemoteSpaceship> remoteShips;
            std::vector <spPlanet> planets;
        } actors;

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
        /// Join existing game
        void onJoinGame (Event * event);
    };

    DECLARE_SMART(GameScene, spGameScene);
}

#endif //CAPSTONE_GAMESCENE_H

/* Copyright © 2022 Aaron Alef */
