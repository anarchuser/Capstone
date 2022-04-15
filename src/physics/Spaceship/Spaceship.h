#ifndef CAPSTONE_SPACESHIP_H
#define CAPSTONE_SPACESHIP_H

#include "config.h"
#include "game_config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "../helper.h"

namespace ph {
    using namespace oxygine;

    class Spaceship: public Sprite {
    private:
        b2Body * body = nullptr;

        bool accelerate  = false;
        bool decelerate  = false;
        bool rotateLeft  = false;
        bool rotateRight = false;

    public:
        Spaceship (b2World * world, oxygine::ResAnim * animation, Vector2 const & pos, float scale = 1);

        void onSteeringEvent (ox::KeyEvent * event);

        void update (UpdateState const & us) override;
    };

    DECLARE_SMART(Spaceship, spSpaceship);

    static spSpaceship instance;
}

#endif //CAPSTONE_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
