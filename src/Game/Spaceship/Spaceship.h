#ifndef CAPSTONE_SPACESHIP_H
#define CAPSTONE_SPACESHIP_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Game/World/World.h"
#include "Game/Collision/CollisionEvent.h"
#include "Game/UI/Text.h"

#define SPACESHIP_SCALE     2e-1

#define SPACESHIP_DENSITY   2e0

#define SPACESHIP_FORCE     5e-4

#define SPACESHIP_TORQUE    3e-5

#define SPACESHIP_HEALTH    100

namespace kt {
    using namespace oxygine;

    class Spaceship: public Sprite {
        int health = SPACESHIP_HEALTH;
        std::size_t id = ship_counter++;
        spText scoreboard = nullptr;

    protected:
        b2Body * body = nullptr;

        bool accelerate  = false;
        bool decelerate  = false;
        bool rotateLeft  = false;
        bool rotateRight = false;

        void updateScoreboard ();

    public:
        Spaceship (World & world, oxygine::ResAnim * animation, Vector2 const & pos, float scale = 1);

        void update (UpdateState const & us) final;

        void setAwake (bool awake);

        static std::size_t ship_counter;
    };

    DECLARE_SMART(Spaceship, spSpaceship);
}

#endif //CAPSTONE_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
