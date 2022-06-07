#ifndef CAPSTONE_SCENE_H
#define CAPSTONE_SCENE_H

#include "config.h"

#include "oxygine-framework.h"

namespace kt {
    using namespace oxygine;

    /// Parent class for Scenes. A scene is a specific state of the game, e.g., in game (GameScene) or menu (MenuScene)
    class Scene: public Actor {
    public:
        /// Construct a new Scene. Injects itself into the stage. Should be the only scene there
        Scene ();
    };
    DECLARE_SMART (Scene, spScene);

} // kt


#endif //CAPSTONE_SCENE_H

/* Copyright © 2022 Aaron Alef */
