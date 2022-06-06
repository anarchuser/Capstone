#include "GameScene.h"

namespace kt {
    GameScene::GameScene() : GameScene (generateSeed()) {}

    GameScene::GameScene (std::string const & ip, unsigned short port): GameScene (requestSeed (ip, port)) {
        joinGame (ip, port);
    }

    GameScene::GameScene (std::size_t seed)
            : Scene()
            , rng (seed)
            , backend {seed, SERVER_FULL_ADDRESS}
            , client {SERVER_FULL_ADDRESS}
            , waitscope {client.getWaitScope()}
            , handle {[this] () {
                // Connect to our own backend, storing the returned handles
                auto request = client.getMain <::Backend>().connectRequest();
                request.setId (CLIENT_ID);
                request.setRegistrar (getRegistrarImpl());
                auto result = request.send().wait (waitscope);
                KJ_REQUIRE (result.hasSynchro());
                KJ_REQUIRE (result.hasRegistrar());
                return Handle {result.getRegistrar(), result.getSynchro()};
            }()}
            {

        logs::messageln ("Seed: %lu", rng.seed);

        // Load all required game assets
        gameResources.loadXML (GAME_RESOURCES);

        // Create the world
        auto & world = actors.world = new World (gameResources.getResAnim ("sky"), WORLD_SIZE);
        addChild (world);

        // Generate a couple of planets, number based on world size
        auto planetAnimation = gameResources.getResAnim ("venus");
        for (std::size_t i = 0; i < PLANETS_PER_PIXEL * world->world_size.x * world->world_size.y; i++) {
            actors.planets.emplace_back (new Planet (* world, planetAnimation, {
                    float (rng.random ({100, world->getSize().x - 100})),
                    float (rng.random ({100, world->getSize().y - 100}))
            }, float (rng.random ({0.3, 0.7}))));
        }

        // Create the keyboard-controlled spaceship with ID = 0
        Spaceship::resetCounter();
        auto & ship = actors.localShip = new KeyboardSpaceship (* world, & gameResources, USERNAME);

        // Register the keyboard-controlled spaceship to the backend
        auto request = handle.registrar.registerShipRequest();
        ship->getData().initialise (request.initShip());
        request.setHandle (ship->getHandle());
        handle.keyboard_sink.emplace (request.send().wait (waitscope).getSink());
        ship->setOnUpdate ([this] (cg::Item const & item) {
            if (!handle.keyboard_sink.has_value()) return;
            auto request = handle.keyboard_sink->sendItemRequest ();
            item.initialise (request.initItem ());
            request.send ().wait (waitscope);
        });
        ship->setOnDone ([this] () {
            actors.localShip = nullptr;
            if (!handle.keyboard_sink.has_value()) return;
            handle.keyboard_sink->doneRequest().send().wait (waitscope);
        });

        // On Escape, open an in-game menu
        getStage()->addEventListener (KeyEvent::KEY_DOWN, [this] (Event * event) {
            auto * keyEvent = (KeyEvent *) event;
            auto keysym = keyEvent->data->keysym;
            switch (keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    onMenu (event);
                    break;
            }
        });
    }

    kj::Own <cg::RegistrarImpl> GameScene::getRegistrarImpl () {
        auto registrar = kj::heap <cg::RegistrarImpl> (CLIENT_ID);
        registrar->setOnRegisterShip ([this] (cg::Spaceship const & data, cg::ClientID const & id, cg::ShipHandle_t handle) -> kj::Own <cg::ShipSinkImpl> {
            try {
                auto & username = data.username;
                OX_ASSERT (username != "Planet");

                // Case 1: the registered ship is the keyboard-controlled original one
                if (auto ship = actors.localShip) {
                    if (ship->getName () == username) {
                        ship->setData (data);
                        ship->setHandle (std::move (handle));
                        return ship->getSink ();
                    }
                }
                // Case 2a: A remote ship with the given name is already registered -> destroy the existing one
                std::remove_if (actors.remoteShips.begin(), actors.remoteShips.end(), [username] (spRemoteSpaceship const & ship) {
                    return ship && ship->getName() == username;
                });

                // Case 2b: Create a new remote ship connected to the given handle
                spRemoteSpaceship ship = new RemoteSpaceship (* actors.world, & gameResources, username);
                actors.remoteShips.push_back (ship);
                ship->setData (data);
                ship->setHandle (std::move (handle));
                return ship->getSink ();
            } catch (std::exception & e) {
                logs::warning ("Error on registering new spaceship");
                return {};
            }
        });
        return registrar;
    }

    void GameScene::update (UpdateState const & us) {
        static bool gameOver = false;
        if (!gameOver) {
            if (!actors.localShip) {
                // The keyboard-controlled instance got destroyed: open the menu
                gameOver = true;
                onMenu (nullptr);
            }
        }

        // All ships were destroyed: stop the game instance
        if (!actors.localShip && actors.remoteShips.empty()) onDisconnect (nullptr);

        Actor::update (us);
    }

    void GameScene::onMenu (Event * event) {
        // Open a dialog with options to join a remote game or restart or quit the current one
        static auto size = getSize();
        static spDialog dialog = [this]() {
            auto dialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Exit the game?");
            dialog->addButton ("Join", CLOSURE (this, & GameScene::onJoinGame));
            dialog->addButton ("Restart", CLOSURE (this, & GameScene::onRestart));
            dialog->addButton ("New game", CLOSURE (this, & GameScene::onNewGame));
            dialog->addButton ("Disconnect", CLOSURE (this, & GameScene::onDisconnect));
            dialog->addButton ("Quit", CLOSURE (this, & GameScene::onQuit));
            dialog->addButton ("Cancel", CLOSURE (this, & GameScene::onMenu));
            return dialog;
        }();

        if (getLastChild() == dialog) removeChild (dialog);
        else addChild (dialog);
    }

    GameScene::~GameScene() noexcept {
        // Free all game assets
        gameResources.free();

        // Destroy the keyboard-controlled ship if it still exists
        if (auto & ship = actors.localShip) ship->destroy();
        actors.localShip = nullptr;

        // Destroy all remote ships
        for (auto & ship : actors.remoteShips) if (ship) ship->destroy();
        actors.remoteShips.clear();

        // Remove this scene including all its listeners
        detach();
        getStage()->removeAllEventListeners();
    }

    void GameScene::onRestart (Event * event) {
        GameScene::~GameScene();
        new GameScene (rng.seed);
    }

    void GameScene::onNewGame (Event * event) {
        GameScene::~GameScene();
        new GameScene (RANDOM_SEED);
    }

    void GameScene::onDisconnect (Event * event) {
        GameScene::~GameScene();
        new MenuScene();
    }

    void GameScene::onQuit (Event * event) {
        GameScene::~GameScene();
        core::requestQuit();
    }

    void GameScene::onJoinGame (Event * event) {
        // Open a dialog asking for an address to connect to
        static auto size = getSize ();
        static spDialog dialog = [this] () {
            auto dialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Enter ip to connect to:");
            dialog->addInput (REMOTE_ADDRESS, [this] (std::string msg) { joinGame (msg, SERVER_PORT); });
            dialog->addButton ("Cancel", CLOSURE (this, & GameScene::onJoinGame));
            return dialog;
        } ();

        if (getLastChild () != dialog) addChild (dialog);
        else removeChild (dialog);
    }
    void GameScene::joinGame (std::string const & ip, unsigned short port) {
        // Ignore if we've already joined this client
        if (!Backend::ping (ip, port)) return;

        // Create a new client to the given address
        auto [iterator, success] = remoteClients.emplace (ip, kj::heap <capnp::EzRpcClient> (ip, port));
        OX_ASSERT (success);

        // Send a join request to the remote backend
        auto & remote = * iterator->second;
        auto request = remote.getMain <::Backend>().joinRequest();
        // Set our own synchro instance as payload
        request.setId (CLIENT_ID);
        request.setRemote (handle.synchro);
        request.send().wait (remote.getWaitScope());
    }
    std::size_t GameScene::requestSeed (std::string const & ip, unsigned short port) {
        // Retrieve the seed of the backend of the address given
        auto client = capnp::EzRpcClient (ip, port);
        auto promise = client.getMain <::Backend>().seedRequest().send();
        return promise.wait (client.getWaitScope()).getSeed();
    }
}

/* Copyright © 2022 Aaron Alef */
