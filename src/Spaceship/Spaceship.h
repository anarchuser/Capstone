#ifndef CAPSTONE_SPACESHIP_H
#define CAPSTONE_SPACESHIP_H

#include "config.h"
#include "helper.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include "src/World/World.h"
#include "src/Collision/CollisionEvent.h"
#include "src/UI/Text.h"

#include "Network/config.h"
#include "Network/Backend.h"
#include "Data/Direction.h"
#include "Data/Spaceship.h"

#include <chrono>
#include <optional>

/// Relative spaceship size
#define SPACESHIP_SCALE     2e-1

/// Density of spaceship. Determines its inertia
#define SPACESHIP_DENSITY   2e0

/// Force applied when issuing acceleration
#define SPACESHIP_FORCE     5e-4

/// Torque applied when turning
#define SPACESHIP_TORQUE    3e-5

/// Number of collisions allowed before destroying spaceship
#define SPACESHIP_HEALTH    2e1

namespace kt {
    using namespace oxygine;

    /// Base spaceship class converting direction commands to force. No control mechanisms included
    class Spaceship: public Sprite {
    private:
        /// Counter incrementing IDs. Reset on starting a new game
        static std::size_t ship_counter;

        /// ID of this spaceship, incremented starting from 0. Used only to determine HUD position
        std::size_t const id = ship_counter++;

        /// HUD showing current health of this ship
        spText scoreboard = nullptr;

        /// Current issued direction:
        cg::Direction direction;

        /// Remote, mainly used to check if the ship is still connected
        std::optional <cg::ShipHandle_t> remote;

        bool isDestroyed = false;

    protected:
        /// Callback to send directions & state to the backend
        cg::SendItemCallback onUpdate;
        /// Callback to tell the backend that we died
        cg::DoneCallback onDone;

        /// A list of all listeners so they can be removed from the stage on death
        std::vector <int> listeners;

        /// Health of this particular instance
        int health = SPACESHIP_HEALTH;

        /// Physical body representing this spaceship in the physical world
        b2Body * body = nullptr;

        /// Update personal scoreboard based on current health
        void updateScoreboard (std::string const & msg = "", long ping = -1);
        /// Calculate the latency to its remote counterpart
        void updatePing();

    public:
        /// Construct a new ship in the current world, at given position (usually centred)
        Spaceship (World & world, Resources * res, std::string const & username);
        /// Default non-throwing dtor as demanded by the base class
        ~Spaceship() noexcept override;

        /// Apply linear or angular impulses based on command flags
        void update (UpdateState const & updateState) override;

        /// Wakes or puts a spaceship to sleep. Asleep spaceships have collision and gravity turned off
        void setAwake (bool awake);

        /// Replace current directions with new ones
        void updateDirection (cg::Direction new_dir);

        /// Destroy this spaceship and remove from the world
        kj::Promise <void> destroy ();

        /// Return current position in physical coordinates
        b2Vec2 getPhysicalPosition () const;
        /// Return current velocity in physical coordinates
        b2Vec2 getPhysicalVelocity () const;

        /// Update current physical position and direction
        void setPhysicalTransform (b2Vec2 pos, float angle);
        /// Update current physical velocity
        void setPhysicalVelocity (b2Vec2 vel);

        /// Return all data necessary to reconstruct this ship's current state
        cg::Spaceship getData () const;
        /// Force-set this ship's state
        void setData (cg::Spaceship const & spaceship);

        /// Configure a handle for direction and state updates
        virtual kj::Own <cg::ShipSinkImpl> getSink();
        /// Configure an overall handle to this ship
        virtual kj::Own <cg::ShipHandleImpl> getHandle();
        /// Give the ship a reference to its remote counterpart
        void setHandle (cg::ShipHandle_t && remote);

        /// Update how to behave on update
        void setOnUpdate (cg::SendItemCallback && onUpdate);
        /// Update the callback for when we crash
        void setOnDone (cg::DoneCallback && onDone);

        /// Counter incrementing IDs. Reset on creating a new GameScene instance
        static void resetCounter();
    };

    DECLARE_SMART(Spaceship, spSpaceship);
}

#endif //CAPSTONE_SPACESHIP_H

/* Copyright © 2022 Aaron Alef */
