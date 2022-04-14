#ifndef CAPSTONE_MAINACTOR_H
#define CAPSTONE_MAINACTOR_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "physics/Spaceship/Spaceship.h"

namespace ph {
    using namespace oxygine;

    class MainActor : public Actor {
    private:
        b2World world;

    public:
        static Resources gameResources;

        MainActor();
        ~MainActor() noexcept override;

        void doUpdate (const UpdateState& us) override;
    };

    DECLARE_SMART(MainActor, spMainActor);
}

#endif //CAPSTONE_MAINACTOR_H

/* Copyright © 2022 Aaron Alef */
