#ifndef CAPSTONE_GAMESCENE_H
#define CAPSTONE_GAMESCENE_H

#include "Network/config.h"
#include "config.h"
#include "env.h"

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
        RNG rng;

        /* All objects related to the connection to the backend */
        /// A wrapper around the running backend, stopping it on destruction
        Backend backend;
        /// A list of all clients we connected to, to prevent concurrent connections to the same address
        std::unordered_map <std::string, kj::Own <capnp::EzRpcClient>> remoteClients;
        /// The handle to our own backend
        capnp::EzRpcClient client;
        /// The wait scope of our own client
        kj::WaitScope & waitscope;

        /// A struct holding several handles to the backend
        struct Handle {
            /// Registrar used to register new ships
            cg::Registrar_t registrar;
            /// Reference to our own synchro instance, to send to remote clients
            cg::Synchro_t synchro;
            /// Sink to pour keyboard events into
            std::optional <cg::ShipSink_t> keyboard_sink;
        } handle;

        /* References to all objects stored in the world */
        /// A struct holding the different actors in one game
        struct Actors {
            /// The world holding all things and updating physics
            spWorld world;
            /// The ship controlled by WASD / arrow keys
            spKeyboardSpaceship localShip;
            /// The list of all remote ships registered by the backend
            std::vector <spRemoteSpaceship> remoteShips;
            /// All planets in the game
            std::vector <spPlanet> planets;
        } actors;

        /// Menu opened on pressing Escape in-game
        spDialog onMenuDialog;

        /// Send a seedRequest to the given address
        static std::size_t requestSeed (std::string const & ip, unsigned short port) ;
        /// Request connection to a remote client
        void joinGame (std::string const & ip, unsigned short port);

        /// Helper function to inialise the world
        void initWorld ();

        /// Helper function to create a registrar, handling new ships joining the game
        kj::Own <cg::RegistrarImpl> getRegistrarImpl ();

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

    public:
        /// Inject a new Game instance with random seed into the stage
        GameScene ();
        /// Inject a new Game instance with given seed into the stage
        explicit GameScene (std::size_t seed);
        /// Directly ping to an existing Game remotely
        GameScene (std::string const & ip, unsigned short port);
        ~GameScene() noexcept override;

        /// Updates its children, given that the game is not paused (i.e., hard and soft pause set to false)
        void update(UpdateState const & updateState) override;
    };

    DECLARE_SMART(GameScene, spGameScene);
}

#endif //CAPSTONE_GAMESCENE_H

/* Copyright © 2022 Aaron Alef */
