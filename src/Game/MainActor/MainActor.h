#ifndef CAPSTONE_MAINACTOR_H
#define CAPSTONE_MAINACTOR_H

#include "config.h"
#include "Game/game_config.h"

#include "oxygine-framework.h"
#include "box2d.h"

namespace kt {
    using namespace oxygine;

    class MainActor : public Actor {
    private:
        Resources gameResources;

    public:
        MainActor();
        ~MainActor() noexcept override;

    };

    DECLARE_SMART(MainActor, spMainActor);
}

#endif //CAPSTONE_MAINACTOR_H

/* Copyright © 2022 Aaron Alef */
