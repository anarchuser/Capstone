#include "MenuScene.h"

namespace kt {
    MenuScene::MenuScene (): Scene() {
        spDialog dialog = new Dialog ({0, 0}, getStage()->getSize(), "Menu");
        dialog->addButton ("New Game", CLOSURE (this, & MenuScene::onNewGame));
        dialog->addButton ("Exit", CLOSURE (this, & MenuScene::onRequestExit));
        addChild (dialog);

        addEventListener (KeyEvent::KEY_DOWN, [this](Event * event) {
            logs::messageln ("Key Down on Menu");
            event->stopsImmediatePropagation = true;
            auto * keyEvent = (KeyEvent *) event;
            auto keysym = keyEvent->data->keysym;
            switch (keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    onRequestExit (event);
                    break;
            }
        });
    }

    void MenuScene::onNewGame (Event * event) {
        getStage()->removeChildren();
        getStage()->removeAllEventListeners();
        new GameScene (RANDOM_SEED);
        detach();
    }
    void MenuScene::onRequestExit (Event * event) {
        core::requestQuit();
    }
}

/* Copyright © 2022 Aaron Alef */
