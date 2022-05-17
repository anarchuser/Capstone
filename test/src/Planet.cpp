#include <catch2/catch_test_macros.hpp>

#include "Algebra/algebra.h"

#include "Planet/Planet.h"
#include "World/World.h"
#include "Spaceship/Spaceship.h"

#define WINDOW_SIZE 100
#define TIME_STEPS 30

SCENARIO ("Planets exert gravitational pull on Spaceships") {
    GIVEN ("A world containing one Planet and one Spaceship") {
        Stage::instance = new Stage;
        getStage()->setSize (WINDOW_SIZE, WINDOW_SIZE);

        b2Vec2 world_size = {10, 10};
        kt::World world (nullptr, world_size);

        b2Vec2 ship_pos = 0.5 * world_size;
        auto & ship = * new kt::Spaceship (world, nullptr, world.convert (ship_pos), SPACESHIP_SCALE);
        auto & p_ship = * (b2Body *) ship.getUserData();

        b2Vec2 planet_pos = 0.1 * ship_pos;
        auto & planet = * new kt::Planet (world, nullptr, world.convert (planet_pos), 0.5);
        auto & p_planet = * (b2Body *) planet.getUserData();

        WHEN ("Time advances") {
            ship.setAwake (true);

            auto speed = p_ship.GetLinearVelocity().Length();
            REQUIRE (speed == 0);

            THEN ("The spaceship accelerates") {
                for (int i = 1; i < TIME_STEPS; i++) {
                    UpdateState us;
                    world.update (us);

                    // TODO: check more things, maybe?
                    auto new_speed = p_ship.GetLinearVelocity().Length();
                    static int j = 0;
                    REQUIRE (new_speed > speed);
                    speed = new_speed;
                }
            }
        }
    }
}

/* Copyright © 2022 Aaron Alef */
