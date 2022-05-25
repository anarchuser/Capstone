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

        spWorld world = new World (gameResources.getResAnim ("sky"), WORLD_SIZE);
        addChild (world);

        // Generate a couple of planets, number based on world size
        auto planetAnimation = gameResources.getResAnim ("venus");
        for (std::size_t i = 0; i < PLANETS_PER_PIXEL * world->world_size.x * world->world_size.y; i++) {
            new Planet (* world, planetAnimation, {
                    float (rng.random ({100, world->getSize().x - 100})),
                    float (rng.random ({100, world->getSize().y - 100}))
            }, float (rng.random ({0.3, 0.7})));
        }

        Spaceship::resetCounter();
        spKeyboardSpaceship ship = new KeyboardSpaceship (* world, & gameResources, USERNAME);

        auto request = handle.registrar.registerShipRequest();
        ship->getData().initialise (request.initShip());
        request.setHandle (ship->getHandle());
        handle.keyboard_sink = std::make_unique <::Backend::ShipSink::Client> (request.send().wait (waitscope).getSink());
        ship->setOnUpdate ([this] (cg::Direction const & direction) {
            auto request = handle.keyboard_sink->sendItemRequest();
            direction.initialise (request.initItem().initDirection());
            return request.send();
        });
        ship->setOnDone ([this] () {
            return handle.keyboard_sink->doneRequest().send();
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
        auto registrar = kj::heap <cg::RegistrarImpl> ();
        registrar->setOnRegisterShip ([this] (cg::Spaceship const & data, ::Backend::ShipHandle::Client const &) {
            spWorld world = safeSpCast <World> (getLastChild());

            logs::messageln ("I am never ever actually called!");

            if (auto ship = KeyboardSpaceship::instance) {
                if (ship->getName() == data.username) {
                    ship->setData (data);
                    return ship->getSink();
                }
            }

            spRemoteSpaceship ship = new RemoteSpaceship (* world, & gameResources, data.username);
            ship->setData (data);
            return ship->getSink();
        });
        return registrar;
    }

    GameScene::~GameScene() noexcept {
        // Free all game assets
        gameResources.free();
        if (auto * ship = KeyboardSpaceship::instance) ship->destroy();
    }

    void GameScene::update (UpdateState const & us) {
        if (!KeyboardSpaceship::instance) {
            ONCE (onMenu (nullptr));
        }

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

        if (getLastChild() == dialog) {
            removeChild (dialog);
        } else {
            addChild (dialog);
        }
    }

    void GameScene::onRestart (Event * event) {
        detach();
        getStage()->removeAllEventListeners();
        this->~GameScene();
        while (get_pointer (getStage()->getLastChild()) == this);
        new GameScene (rng.seed);
    }

    void GameScene::onNewGame (Event * event) {
        detach();
        getStage()->removeAllEventListeners();
        this->~GameScene();
        while (get_pointer (getStage()->getLastChild()) == this);
        new GameScene (RANDOM_SEED);
    }

    void GameScene::onDisconnect (Event * event) {
        detach();
        getStage()->removeAllEventListeners();
        this->~GameScene();
        while (get_pointer (getStage()->getLastChild()) == this);
        new MenuScene();
    }

    void GameScene::onQuit (Event * event) {
        core::requestQuit();
    }

    std::size_t GameScene::requestSeed (std::string const & ip, unsigned short port) {
        auto client = capnp::EzRpcClient (ip, port);
        auto promise = client.getMain <::Backend>().seedRequest().send();
        return promise.wait (client.getWaitScope()).getSeed();
    }

    void GameScene::joinGame (std::string const & ip, unsigned short port) {
        remoteClients.emplace_back (std::make_unique <capnp::EzRpcClient> (ip, port));
        auto & remote = * remoteClients.back();
        auto remoteRequest = remote.getMain <::Backend>().joinRequest();
        remoteRequest.setRemote (handle.synchro);
        auto remoteSynchro = remoteRequest.send().wait (remote.getWaitScope()).getLocal();

        auto localRequest = client.getMain <::Backend>().joinRequest();
        localRequest.setRemote (remoteSynchro);
        localRequest.send().wait (waitscope);
    }
}

/* Copyright © 2022 Aaron Alef */
