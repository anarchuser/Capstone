#include "GameScene.h"

namespace kt {
    GameScene::GameScene () : GameScene (generateSeed ()) {}

    GameScene::GameScene (std::string const & ip, unsigned short port): GameScene (requestSeed (ip, port), true) {
        joinGame (ip, port);
    }

    GameScene::GameScene (std::size_t seed, bool join)
            : Scene ()
            , rng (seed)
            , backend {seed, SERVER_FULL_ADDRESS}
            {

        logs::messageln ("Seed: %lu", rng.seed);

        // Load all required game assets
        gameResources.loadXML (GAME_RESOURCES);

        spWorld world = new World (gameResources.getResAnim ("sky"), WORLD_SIZE);
        addChild (world);
        world->onSendSink = [&] (std::string const & username) -> kj::Own <cg::ItemSinkImpl> {
            logs::messageln ("Received sink for spaceship '%s'", username.c_str());

            if (KeyboardSpaceship::instance && KeyboardSpaceship::instance->getName() == username)
                return kj::heap <cg::ItemSinkImpl> (
                        CLOSURE (KeyboardSpaceship::instance, & KeyboardSpaceship::destroy),
                        CLOSURE (KeyboardSpaceship::instance, & KeyboardSpaceship::updateDirection));

            spWorld world = World::instance;
            OX_ASSERT (world);
            spActor child = world->getChild (username, oxygine::ep_ignore_error);
            OX_ASSERT (! child);

            spRemoteSpaceship ship = new RemoteSpaceship (* world, gameResources, getSize() * 0.5, SPACESHIP_SCALE);
            ship->setName (username);
            return kj::heap <cg::ItemSinkImpl> (
                    CLOSURE (ship.get(), & RemoteSpaceship::destroy),
                    CLOSURE (ship.get(), & RemoteSpaceship::updateDirection));
        };

        // Generate a couple of planets, number based on world size
        auto planetAnimation = gameResources.getResAnim ("venus");
        for (std::size_t i = 0; i < PLANETS_PER_PIXEL * world->world_size.x * world->world_size.y; i++) {
            new Planet (* world, planetAnimation, {
                    float (rng.random ({100, world->getSize ().x - 100})),
                    float (rng.random ({100, world->getSize ().y - 100}))
            }, float (rng.random ({0.3, 0.7})));
        }

        Spaceship::ship_counter = 0;
        if (!join)
            new KeyboardSpaceship (* world, gameResources, getSize() * 0.5, SPACESHIP_SCALE, SERVER_FULL_ADDRESS);

        getStage ()->addEventListener (KeyEvent::KEY_DOWN, [this] (Event * event) {
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

    GameScene::~GameScene () noexcept {
        // Free all game assets
        gameResources.free ();
    }

    void GameScene::update (UpdateState const & us) {
        // TODO: don't pause in multiplayer games
//        if (hardPause) return;
//        if (softPause) return;

        if (!KeyboardSpaceship::instance) {
            ONCE (onMenu (nullptr));
        }

        Actor::update (us);
    }

    void GameScene::onMenu (Event * event) {
        hardPause = !hardPause;
        static auto size = getSize ();
        static spDialog dialog = [this] () {
            auto dialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Exit the game?");
            dialog->addButton ("Restart", CLOSURE (this, & GameScene::onRestart));
            dialog->addButton ("New game", CLOSURE (this, & GameScene::onNewGame));
            dialog->addButton ("Disconnect", CLOSURE (this, & GameScene::onDisconnect));
            dialog->addButton ("Quit", CLOSURE (this, & GameScene::onQuit));
            dialog->addButton ("Cancel", CLOSURE (this, & GameScene::onMenu));
            return dialog;
        } ();

        // TODO: check if *any* child is dialog
        if (getLastChild () != dialog) {
            addChild (dialog);
        } else {
            removeChild (dialog);
        }
    }

    void GameScene::onRestart (Event * event) {
        detach ();
        getStage ()->removeAllEventListeners ();
        while (get_pointer (getStage ()->getLastChild ()) == this);
        new GameScene (rng.seed);
    }

    void GameScene::onNewGame (Event * event) {
        detach ();
        getStage ()->removeAllEventListeners ();
        while (get_pointer (getStage ()->getLastChild ()) == this);
        new GameScene (RANDOM_SEED);
    }

    void GameScene::onDisconnect (Event * event) {
        detach ();
        getStage ()->removeAllEventListeners ();
        while (get_pointer (getStage ()->getLastChild ()) == this);
        new MenuScene ();
    }

    void GameScene::onQuit (Event * event) {
        core::requestQuit ();
    }

    std::size_t GameScene::requestSeed (std::string const & ip, unsigned short port) {
        auto client = capnp::EzRpcClient (ip, port);
        auto promise = client.getMain <Synchro> ().seedRequest ().send();
        return promise.wait (client.getWaitScope()).getSeed();
    }

    void GameScene::joinGame (std::string const & ip, unsigned short port) {
        if (!KeyboardSpaceship::instance) {
            new KeyboardSpaceship (* safeSpCast <World> (getFirstChild()), gameResources, getSize() * 0.5, SPACESHIP_SCALE, ip + ":" + std::to_string (port));
        } else {
            logs::warning ("Joining Game whilst Spaceship already present");
        }
    }
}

/* Copyright © 2022 Aaron Alef */
