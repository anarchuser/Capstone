#ifndef CAPSTONE_MAINACTOR_H
#define CAPSTONE_MAINACTOR_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include <cmath>

#include "physics/Spaceship/Spaceship.h"
#include "physics/Planet/Planet.h"
#include "physics/box2d/Box2DDebugDraw.h"
#include "random/random.h"

namespace ph {
    using namespace oxygine;

    class MainActor : public Actor {
    private:
        b2World world;
        spBox2DDraw _debugDraw;

        Vector2 getRandomPos ();

    public:
        static Resources gameResources;
        static unsigned int const seed;

        MainActor();
        ~MainActor() noexcept override;

        void addPlanet ();

        void doUpdate (const UpdateState& us) override;

        void toggleDebugDraw();

        static unsigned int genSeededSeed ();
    };

    DECLARE_SMART(MainActor, spMainActor);
}

#endif //CAPSTONE_MAINACTOR_H

/* Copyright © 2022 Aaron Alef */
