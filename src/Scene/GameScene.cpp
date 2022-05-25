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
            , synchro {[&] () {
                auto request = client.getMain <::Backend>().connectRequest();
                request.setClient (getSubscriberImpl());
                auto result = request.send().wait (client.getWaitScope());
                KJ_REQUIRE (result.hasSynchro());
                return result.getSynchro();
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

        getStage()->addEventListener (KeyEvent::KEY_DOWN, [this] (Event * event) {
            auto * keyEvent = (KeyEvent *) event;
            auto keysym = keyEvent->data->keysym;
            switch (keysym.scancode) {
                case SDL_SCANCODE_P:
                    softPause = !softPause;
                    break;
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_UP:
                case SDL_SCANCODE_LEFT:
                case SDL_SCANCODE_DOWN:
                case SDL_SCANCODE_RIGHT:
                    softPause = false;
                    break;
                case SDL_SCANCODE_ESCAPE:
                    onMenu (event);
                    break;
            }
        });
    }

    kj::Own <cg::SubscriberImpl> GameScene::getSubscriberImpl () {
        auto subscriber = kj::heap <cg::SubscriberImpl> ();
        return subscriber;
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
        hardPause = !hardPause;
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
        remoteRequest.setRemote (synchro);
        auto remoteSynchro = remoteRequest.send().wait (remote.getWaitScope()).getLocal();

        auto localRequest = client.getMain <::Backend>().joinRequest();
        localRequest.setRemote (remoteSynchro);
        localRequest.send().wait (waitscope);
    }
}

/* Copyright © 2022 Aaron Alef */
