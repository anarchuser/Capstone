#include "GameScene.h"

namespace kt {
    GameScene::GameScene (): GameScene (generateSeed()) {}
    GameScene::GameScene (std::size_t seed): Scene (), rng (seed) {
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

        auto spaceshipAnimation = gameResources.getResAnim ("spaceship");
        new KeyboardSpaceship (* world, spaceshipAnimation, world->convert (0.5 * world->world_size), SPACESHIP_SCALE);

        getStage()->addEventListener (KeyEvent::KEY_DOWN, [this](Event * event) {
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
        gameResources.free();
    }

    void GameScene::update (UpdateState const & us) {
        if (hardPause) return;
        if (softPause) return;

        Actor::update (us);
    }

    void GameScene::onMenu (Event * event) {
        hardPause = !hardPause;
        static auto size = getSize();
        static spDialog dialog = [this] () {
            auto dialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Exit the game?");
            dialog->addButton ("Restart", CLOSURE (this, & GameScene::onRestart));
            dialog->addButton ("New game", CLOSURE (this, & GameScene::onNewGame));
            dialog->addButton ("Abandon", CLOSURE (this, & GameScene::onAbandon));
            dialog->addButton ("Quit",  CLOSURE (this, & GameScene::onQuit));
            dialog->addButton ("Cancel",  CLOSURE (this, & GameScene::onMenu));
            return dialog;
        }();

        if (getLastChild() != dialog) {
            addChild (dialog);
        } else {
            removeChild (dialog);
        }
    }

    void GameScene::onRestart (Event * event) {
        detach();
        getStage()->removeAllEventListeners();
        while (get_pointer(getStage()->getLastChild()) == this);
        new GameScene (rng.seed);
    }

    void GameScene::onNewGame (Event * event) {
        detach();
        getStage()->removeAllEventListeners();
        while (get_pointer(getStage()->getLastChild()) == this);
        new GameScene (RANDOM_SEED);
    }

    void GameScene::onAbandon (Event * event) {
        detach();
        getStage()->removeAllEventListeners();
        while (get_pointer(getStage()->getLastChild()) == this);
        new MenuScene ();
    }
    
    void GameScene::onQuit (Event * event) {
        core::requestQuit();
    }
}

/* Copyright © 2022 Aaron Alef */
