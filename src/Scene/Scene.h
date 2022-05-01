#ifndef CAPSTONE_SCENE_H
#define CAPSTONE_SCENE_H

#include "config.h"

#include "oxygine-framework.h"

#include <functional>

namespace kt {
    using namespace oxygine;

    DECLARE_SMART (Scene, spScene);

    /// Parent class for Scenes. A scene is a specific state of the game, e.g., in game (GameScene) or menu (MenuScene)
    class Scene: public Actor {
    public:
        /// Construct a new scene. Injects itself into the stage. Should optimally be the only scene there
        Scene ();
    };

} // kt


#endif //CAPSTONE_SCENE_H

/* Copyright © 2022 Aaron Alef */
