#include "MenuScene.h"

namespace kt {
    std::string MenuScene::username = HOSTNAME_AS_USERNAME ? hostname() : ::username();

    MenuScene::MenuScene (): Scene() {
        // Create a new main menu dialog
        spDialog dialog = new Dialog ({0, 0}, getStage()->getSize(), "Menu");
        dialog->addInput (username, MenuScene::changeUsername);
        dialog->addButton ("New Game", CLOSURE (this, & MenuScene::onNewGame));
        dialog->addButton ("Join Game", CLOSURE (this, & MenuScene::onJoinGame));
        dialog->addButton ("Join Directly", [this] (Event * event) { joinGame (REMOTE_ADDRESS); });
        dialog->addButton ("Exit", & MenuScene::onRequestExit);
        addChild (dialog);

        auto size = getSize ();
        onJoinDialog = new Dialog ({size.x / 4, size.y / 5}, {size.x / 2, size.y / 2}, "Enter ip to join:");
        onJoinDialog->addInput (REMOTE_ADDRESS, LAMBDA (joinGame));
        onJoinDialog->addButton ("Cancel", LAMBDA (onJoinGame));

        // Quit the game if Escape is pressed
        getStage()->addEventListener (KeyEvent::KEY_DOWN, [] (Event * event) {
            auto * keyEvent = (KeyEvent *) event;
            auto keysym = keyEvent->data->keysym;
            if (keysym.scancode == SDL_SCANCODE_ESCAPE) onRequestExit (event);
        });
    }

    MenuScene::~MenuScene () {
        getStage()->removeAllEventListeners();
        detach();
    }

    void MenuScene::onNewGame (Event * event) {
        MenuScene::~MenuScene();
        new GameScene (RANDOM_SEED);
    }
    void MenuScene::onJoinGame (Event * event) {
        // Request entering an address to connect to
        if (getLastChild() == onJoinDialog) removeChild (onJoinDialog);
        else addChild (onJoinDialog);
    }
    void MenuScene::joinGame (std::string const & address) {
        // Ping the address; if it resolves start a new game and connect to it
        if (Backend::ping (address, SERVER_PORT)) {
            MenuScene::~MenuScene();
            new GameScene (address, SERVER_PORT);
        }
    }
    void MenuScene::onRequestExit (Event * event) {
        core::requestQuit();
    }

    void MenuScene::changeUsername (std::string const & new_name) {
        username = new_name;
        logs::messageln ("Updated username to '%s'", username.c_str());
    }
    std::string const & MenuScene::getUsername () {
        return username;
    }
}

/* Copyright © 2022 Aaron Alef */
