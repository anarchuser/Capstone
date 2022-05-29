#include "MenuScene.h"

namespace kt {
    MenuScene::MenuScene (): Scene() {
        spDialog dialog = new Dialog ({0, 0}, getStage()->getSize(), "Menu");
        dialog->addButton ("New Game", CLOSURE (this, & MenuScene::onNewGame));
        dialog->addButton ("Join Game", CLOSURE (this, & MenuScene::onJoinGame));
        dialog->addButton ("Join Directly", [this] (Event * event) { joinGame (REMOTE_ADDRESS); });
        dialog->addButton ("Exit", & MenuScene::onRequestExit);
        addChild (dialog);

        getStage()->addEventListener (KeyEvent::KEY_DOWN, [] (Event * event) {
            auto * keyEvent = (KeyEvent *) event;
            auto keysym = keyEvent->data->keysym;
            if (keysym.scancode == SDL_SCANCODE_ESCAPE) onRequestExit (event);
        });
    }

    MenuScene::~MenuScene () {
        detach();
        getStage()->removeAllEventListeners();
    }

    void MenuScene::onNewGame (Event * event) {
        MenuScene::~MenuScene();
        new GameScene (RANDOM_SEED);
    }
    void MenuScene::onJoinGame (Event * event) {
        static auto size = getSize ();
        static spDialog dialog = [this] () {
            auto dialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Enter ip to ping to:");
            dialog->addInput (REMOTE_ADDRESS, [this] (std::string msg) { joinGame (msg); });
            dialog->addButton ("Cancel", CLOSURE (this, & MenuScene::onJoinGame));
            return dialog;
        } ();

        // TODO: check if *any* child is dialog
        if (getLastChild () != dialog) addChild (dialog);
        else removeChild (dialog);
    }
    void MenuScene::joinGame (std::string const & address) {
        if (Backend::ping (address, SERVER_PORT)) {
            MenuScene::~MenuScene();
            new GameScene (address, SERVER_PORT);
        }
    }
    void MenuScene::onRequestExit (Event * event) {
        core::requestQuit();
    }
}

/* Copyright © 2022 Aaron Alef */
