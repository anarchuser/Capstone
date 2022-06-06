#include "Scene.h"

namespace kt {
    Scene::Scene () {
        // Inject us into the stage and adjust position and size to it (-> fill the entire window with this scene)
        attachTo (getStage());
        setPosition (getStage()->getPosition());
        setSize     (getStage()->getSize());
    }
}

/* Copyright © 2022 Aaron Alef */
