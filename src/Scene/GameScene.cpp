#include "GameScene.h"

namespace kt {
    GameScene::GameScene() : GameScene (generateSeed()) {}

    GameScene::GameScene (std::string const & ip, unsigned short port): GameScene (requestSeed (ip, port)) {
        joinGame (ip, port);
    }

    GameScene::GameScene (std::size_t seed)
            : Scene()
            , rng (seed)
            , backend {seed, SERVER_ADDRESS, SERVER_PORT}
            , client {SERVER_ADDRESS, SERVER_PORT}
            , waitscope {client.getWaitScope()}
            , handle {[this] () {
                // Connect to our own backend, storing the returned handles
                auto request = client.getMain <::Backend>().connectRequest();
                request.setId (hostname());
                request.setRegistrar (getRegistrarImpl());
                auto result = request.send().wait (waitscope);
                KJ_REQUIRE (result.hasSynchro());
                KJ_REQUIRE (result.hasRegistrar());
                return Handle {result.getRegistrar(), result.getSynchro()};
            }()}
            {
        logs::messageln ("Seed: %lu", rng.seed);
        initWorld();

        // Register the keyboard-controlled spaceship to the backend
        auto & ship = actors.localShip;
        auto request = handle.registrar.registerShipRequest();
        ship->getData().initialise (request.initShip());
        request.setHandle (ship->getHandle());
        handle.keyboard_sink.emplace (request.send().wait (waitscope).getSink());
        ship->setOnUpdate ([this] (cg::Item const & item) -> kj::Promise <void> {
            if (!handle.keyboard_sink.has_value()) return kj::READY_NOW;
            auto request = handle.keyboard_sink->sendItemRequest ();
            item.initialise (request.initItem ());
            return request.send().ignoreResult();
        });
        ship->setOnDone ([this] () -> kj::Promise <void> {
            actors.localShip = nullptr;
            if (!handle.keyboard_sink.has_value()) return kj::READY_NOW;
            auto promise = handle.keyboard_sink->doneRequest().send().ignoreResult();
            handle.keyboard_sink.reset();
            return promise;
        });

        // Configure dialog to open on pressing Escape
        auto size = getSize();
        onMenuDialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Exit the game?");
        onMenuDialog->addButton ("Restart", CLOSURE (this, & GameScene::onRestart));
        onMenuDialog->addButton ("New game", CLOSURE (this, & GameScene::onNewGame));
        onMenuDialog->addButton ("Disconnect", CLOSURE (this, & GameScene::onDisconnect));
        onMenuDialog->addButton ("Quit", CLOSURE (this, & GameScene::onQuit));
        onMenuDialog->addButton ("Cancel", CLOSURE (this, & GameScene::onMenu));

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

    void GameScene::initWorld () {
        // Load all required game assets
        gameResources.loadXML (GAME_RESOURCES);

        // Create the world
        auto & world = actors.world = new World (gameResources.getResAnim ("sky"), WORLD_SIZE);
        addChild (world);

        // Generate a couple of planets, number based on world size
        auto planetAnimation = gameResources.getResAnim ("venus");
        int const number_of_planets = PLANETS_PER_PIXEL * getSize().x * getSize().y;
        for (std::size_t i = 0; i < number_of_planets; i++) {
            actors.planets.push_back (new Planet (* world, planetAnimation, {
                    float (rng.random ({100, world->getSize().x - 100})),
                    float (rng.random ({100, world->getSize().y - 100}))
            }, float (rng.random ({0.3, 0.7}))));
        }

        // Create the keyboard-controlled spaceship with ID = 0
        Spaceship::resetCounter();
        actors.localShip = new KeyboardSpaceship (* world, & gameResources, MenuScene::getUsername(), waitscope);
    }

    kj::Own <cg::RegistrarImpl> GameScene::getRegistrarImpl () {
        auto registrar = kj::heap <cg::RegistrarImpl> (hostname());
        registrar->setOnRegisterShip ([this] (cg::Spaceship const & data, cg::ClientID const & id, cg::ShipHandle_t handle) -> kj::Own <cg::ShipSinkImpl> {
            try {
                auto & username = data.username;
                OX_ASSERT (username != "Planet");

                // Case 1: the registered ship is the keyboard-controlled original one
                if (auto & ship = actors.localShip) {
                    if (ship->getName () == username) {
                        ship->setData (data);
                        ship->setHandle (std::move (handle));
                        return ship->getSink ();
                    }
                }
                // Case 2a: A remote ship with the given name is already registered -> destroy the existing one
                std::lock_guard<std::mutex> guard (actors.mx);
                actors.remoteShips.erase (username);

                // Case 2b: Create a new remote ship connected to the given handle
                spRemoteSpaceship ship = new RemoteSpaceship (* actors.world, & gameResources, username);
                actors.remoteShips.emplace (username, ship);
                ship->setOnDone ([](){ return kj::READY_NOW; });
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

    void GameScene::update (UpdateState const & updateState) {
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

        Actor::update (updateState);
    }

    void GameScene::onMenu (Event * event) {
        // Open a dialog with options to join a remote game or restart or quit the current one
        if (getLastChild() == onMenuDialog) removeChild (onMenuDialog);
        else addChild (onMenuDialog);
    }

    GameScene::~GameScene() noexcept {
        // Destroy KeyboardSpaceship
        if (auto ship = actors.localShip) ship->destroy().wait (waitscope);

        // Destroy RemoteSpaceships
        for (auto ship : actors.remoteShips) ship.second->destroy().wait (waitscope);
        actors.remoteShips.clear();

        // Free all game assets
        gameResources.free();

        // Remove this scene including all its listeners
        removeChildren();
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
        request.setId (hostname());
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
