#include "MenuScene.h"

namespace kt {
    MenuScene::MenuScene (): Scene() {
        spDialog dialog = new Dialog ({0, 0}, getStage()->getSize(), "Menu");
        dialog->addButton ("New Game", CLOSURE (this, & MenuScene::onNewGame));
        dialog->addButton ("Join Game", CLOSURE (this, & MenuScene::onJoinGame));
        dialog->addButton ("Join Directly", [this] (Event * event) {
            joinGame (REMOTE_ADDRESS);
        });
        dialog->addButton ("Exit", CLOSURE (this, & MenuScene::onRequestExit));
        addChild (dialog);

        getStage()->addEventListener (KeyEvent::KEY_DOWN, [this](Event * event) {
            auto * keyEvent = (KeyEvent *) event;
            auto keysym = keyEvent->data->keysym;
            switch (keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    event->stopImmediatePropagation();
                    onRequestExit (event);
                    break;
            }
        });
    }

    void MenuScene::onNewGame (Event * event) {
        detach();
        getStage()->removeAllEventListeners();
        this->~MenuScene();
        while (get_pointer(getStage()->getLastChild()) == this);
        new GameScene (RANDOM_SEED);
    }
    void MenuScene::onJoinGame (Event * event) {
        static auto size = getSize ();
        static spDialog dialog = [this] () {
            auto dialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Enter ip to ping to:");
            dialog->addInput (REMOTE_ADDRESS, [this] (std::string msg) {
                if (Backend::ping (msg, SERVER_PORT)) {
                    joinGame (msg);
                }
            });
            dialog->addButton ("Cancel", CLOSURE (this, & MenuScene::onJoinGame));
            return dialog;
        } ();

        // TODO: check if *any* child is dialog
        if (getLastChild () != dialog) {
            addChild (dialog);
        } else {
            removeChild (dialog);
        }
    }
    void MenuScene::joinGame (std::string const & address) {
        detach();
        getStage()->removeAllEventListeners();
//        this->~MenuScene();
        while (get_pointer (getStage()->getLastChild()) == this);
        new GameScene (address, SERVER_PORT);
    }
    void MenuScene::onRequestExit (Event * event) {
        core::requestQuit();
    }
}

/* Copyright © 2022 Aaron Alef */
