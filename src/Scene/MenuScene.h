#ifndef CAPSTONE_MENUSCENE_H
#define CAPSTONE_MENUSCENE_H

#include "config.h"

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
    public:
        /// Inject a new Main Menu scene instance into the stage
        MenuScene ();

        ~MenuScene ();

        /// Callback for when starting a new game is requested
        void onNewGame (Event * event);
        /// Callback for when joining an existing game is requested
        void onJoinGame (Event * event);
        /// Callback for when quitting the application is requested
        static void onRequestExit (Event * event);

        void joinGame (std::string const & address);
    };
    DECLARE_SMART (MenuScene, spMenuScene);

} // kt

#endif //CAPSTONE_MENUSCENE_H

/* Copyright © 2022 Aaron Alef */
