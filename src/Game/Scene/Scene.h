#ifndef CAPSTONE_SCENE_H
#define CAPSTONE_SCENE_H

#include "config.h"

#include "oxygine-framework.h"

#include <functional>

namespace kt {
    using namespace oxygine;

    DECLARE_SMART (Scene, spScene);
    class Scene: public Actor {
    public:
        Scene ();
    };

} // kt


#endif //CAPSTONE_SCENE_H

/* Copyright © 2022 Aaron Alef */
