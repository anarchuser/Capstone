#ifndef CAPSTONE_SPACESHIP_H
#define CAPSTONE_SPACESHIP_H

#include "config.h"
#include "Game/config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "Game/World/World.h"
#include "Game/Collision/CollisionEvent.h"
#include "Game/UI/Text.h"

/// Relative spaceship size
#define SPACESHIP_SCALE     2e-1

/// Density of spaceship. Determines its inertia
#define SPACESHIP_DENSITY   2e0

/// Force applied when issuing acceleration
#define SPACESHIP_FORCE     5e-4

/// Torque applied when turning
#define SPACESHIP_TORQUE    3e-5

/// Number of collisions allowed before destroying spaceship
#define SPACESHIP_HEALTH    100

namespace kt {
    using namespace oxygine;

    /// Spaceship intended to be controlled by (human) players. No control mechanism included; subclass to do so
    class Spaceship: public Sprite {
        /// Health of this particular instance
        int health = SPACESHIP_HEALTH;
        /// ID of this spaceship. IDs increment with each ship, starting from 0 each game
        std::size_t const id = ship_counter++;
        /// HUD showing current health of this ship
        spText scoreboard = nullptr;

    protected:
        /// Physical body representing this spaceship in the physical world
        b2Body * body = nullptr;

        /// Flag issuing acceleration
        bool accelerate  = false;
        /// Flag issuing deceleration
        bool decelerate  = false;
        /// Flag issuing counter clock-wise rotation
        bool rotateLeft  = false;
        /// Flag issuing clock-wise rotation
        bool rotateRight = false;

        /// Update personal scoreboard based on current health
        void updateScoreboard ();

    public:
        /// Construct a new ship in the current world, at given position (usually centred)
        Spaceship (World & world, Resources & res, Vector2 const & pos, float scale = 1);

        /// Apply linear or angular impulses based on command flags
        void update (UpdateState const & us) final;

        /// Wakes or puts a spaceship to sleep. Asleep spaceships have collision and gravity turned off
        void setAwake (bool awake);

        /// Counter incrementing IDs. Reset on creating a new GameScene instance
        static std::size_t ship_counter;
    };

    DECLARE_SMART(Spaceship, spSpaceship);
}

#endif //CAPSTONE_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
