#ifndef CAPSTONE_MENUSCENE_H
#define CAPSTONE_MENUSCENE_H

#include "config.h"
#include "env.h"

#include "oxygine-framework.h"

#include "src/UI/Dialog.h"
#include "src/UI/Input.h"
#include "Scene.h"
#include "GameScene.h"
#include "Backend/Backend.h"

namespace kt {
    using namespace oxygine;

    /// Scene representing the main menu. Consists of a window-spanning dialog with "start game" and "quit" options
    class MenuScene: public Scene {
    private:
        /// Dialog opened on pressing "Join Game"
        spDialog onJoinDialog;

        /// Callback for when starting a new game is requested
        void onNewGame (Event * event);
        /// Callback for when quitting the application is requested
        static void onRequestExit (Event * event);
        /// Callback for when joining an existing game is requested
        void onJoinGame (Event * event);

        /// If the address resolves a ping request connect to it
        void joinGame (std::string const & address);

        static std::string username;
        static void changeUsername (std::string const & new_name);

    public:
        /// Inject a new Main Menu scene instance into the stage
        MenuScene ();
        /// Clean up all traces of this scene
        ~MenuScene ();

        static std::string const & getUsername();
    };
    DECLARE_SMART (MenuScene, spMenuScene);

} // kt

#endif //CAPSTONE_MENUSCENE_H

/* Copyright © 2022 Aaron Alef */
