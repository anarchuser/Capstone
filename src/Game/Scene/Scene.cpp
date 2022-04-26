#include "Scene.h"

namespace kt {
    Scene::Scene () {
        attachTo (getStage());
        setPosition (getStage()->getPosition());
        setSize     (getStage()->getSize());
    }
}

/* Copyright © 2022 Aaron Alef */
