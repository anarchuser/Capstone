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
                auto request = client.getMain <::Backend>().connectRequest();
                request.setId (USERNAME);
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

        Spaceship::resetCounter();
        auto & ship = actors.localShip = new KeyboardSpaceship (* world, & gameResources, USERNAME);


        auto request = handle.registrar.registerShipRequest();
        ship->getData().initialise (request.initShip());
        request.setHandle (ship->getHandle());
        handle.keyboard_sink = std::make_unique <cg::ShipSink_t> (request.send().wait (waitscope).getSink());
        ship->setOnUpdate ([this] (cg::Direction const & direction) {
            auto request = handle.keyboard_sink->sendItemRequest ();
            direction.initialise (request.initItem ().initDirection ());
            request.send ().wait (waitscope);
        });
        ship->setOnDone ([this] () {
            actors.localShip = nullptr;
            handle.keyboard_sink->doneRequest().send().wait (waitscope);
        });

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
        auto registrar = kj::heap <cg::RegistrarImpl> (USERNAME);
        registrar->setOnRegisterShip ([this] (cg::Spaceship const & data, cg::ClientID const & id, cg::ShipHandle_t handle) -> kj::Own <cg::ShipSinkImpl> {
            try {
                OX_ASSERT (data.username != "Planet");

                if (auto ship = actors.localShip) {
                    if (ship->getName () == data.username) {
                        ship->setData (data);
                        ship->setHandle (kj::heap<cg::ShipHandle_t> (handle));
                        return ship->getSink ();
                    }
                }
                for (auto remote = actors.remoteShips.begin(); remote != actors.remoteShips.end(); ++remote) {
                    // If ship with this name exists already, replace it
                    if (spRemoteSpaceship ship = * remote) {
                        if (ship->getName() == data.username) {
                            ship->detach();
                            actors.remoteShips.erase (remote);
                        }
                    }
                }

                spRemoteSpaceship ship = new RemoteSpaceship (* actors.world, & gameResources, data.username);
                actors.remoteShips.push_back (ship);
                ship->setData (data);
                ship->setHandle (kj::heap <cg::ShipHandle_t> (handle));
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
                gameOver = true;
                onMenu (nullptr);
            }
        }

        if (!actors.localShip && actors.remoteShips.empty()) onDisconnect (nullptr);

        Actor::update (us);
    }

    void GameScene::onMenu (Event * event) {
        static auto size = getSize();
        static spDialog dialog = [this]() {
            auto dialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Exit the game?");
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

        detach();
        getStage()->removeAllEventListeners();

        if (auto & ship = actors.localShip) ship->destroy();
        actors.localShip = nullptr;

        for (auto & ship : actors.remoteShips) ship->destroy();
        actors.remoteShips.clear();
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

    std::size_t GameScene::requestSeed (std::string const & ip, unsigned short port) {
        auto client = capnp::EzRpcClient (ip, port);
        auto promise = client.getMain <::Backend>().seedRequest().send();
        return promise.wait (client.getWaitScope()).getSeed();
    }

    void GameScene::joinGame (std::string const & ip, unsigned short port) {
        auto [iterator, success] = remoteClients.emplace (ip, kj::heap <capnp::EzRpcClient> (ip, port));
        auto & remote = * iterator->second;
        auto request = remote.getMain <::Backend>().joinRequest();
        request.setId (USERNAME);
        request.setRemote (handle.synchro);
        request.send().wait (remote.getWaitScope());
    }
}

/* Copyright © 2022 Aaron Alef */
