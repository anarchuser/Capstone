#ifndef CAPSTONE_MENUSCENE_H
#define CAPSTONE_MENUSCENE_H

#include "config.h"

#include "oxygine-framework.h"

#include "Game/UI/Dialog.h"
#include "Game/Scene/Scene.h"
//#include "Game/Scene/config.h"
#include "GameScene.h"

namespace kt {
    using namespace oxygine;

class MenuScene: public Scene {
    public:
        MenuScene ();

        void onNewGame (Event * event);
        void onRequestExit (Event * event);
    };
    DECLARE_SMART (MenuScene, spMenuScene);

} // kt

#endif //CAPSTONE_MENUSCENE_H

/* Copyright © 2022 Aaron Alef */
