#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "Planet/Planet.h"
#include "World/World.h"
#include "Spaceship/Spaceship.h"

#define WINDOW_SIZE 100
#define TIME_STEPS 300

using namespace Catch;

SCENARIO ("Planets exert gravitational pull on Spaceships") {
    GIVEN ("A world containing one Planet and one Spaceship") {
        Stage::instance = new Stage;
        getStage()->setSize (WINDOW_SIZE, WINDOW_SIZE);

        b2Vec2 world_size = {10, 10};
        kt::World world (nullptr, world_size);

        b2Vec2 ship_pos = 0.5 * world_size;
        auto & ship = * new kt::Spaceship (world, nullptr, "");
        auto & p_ship = * (b2Body *) ship.getUserData();

        b2Vec2 planet_pos = 0.1 * ship_pos;
        auto & planet = * new kt::Planet (world, nullptr, world.convert (planet_pos), 0.5);
        auto & p_planet = * (b2Body *) planet.getUserData();
        auto direction = planet_pos - ship_pos;

        WHEN ("Time advances") {
            ship.setAwake (true);

            auto speed = p_ship.GetLinearVelocity().Length();
            auto distance = direction.Normalize();
            REQUIRE (speed == 0);

            THEN ("The spaceship accelerates") {
                for (int i = 1; i < TIME_STEPS; i++) {
                    UpdateState us; us.dt = 10;
                    world.update (us);

                    auto velocity = p_ship.GetLinearVelocity();
                    auto new_speed = velocity.Normalize();
                    CHECK (velocity.x == Approx (direction.x).epsilon (1e-2));
                    CHECK (velocity.y == Approx (direction.y).epsilon (1e-2));
                    CHECK (new_speed > speed);
                    speed = new_speed;

                    auto new_direction = p_planet.GetPosition() - p_ship.GetPosition();
                    auto new_distance = new_direction.Normalize();
                    CHECK (new_distance < distance);
                    CHECK (new_direction.x == Approx (direction.x).epsilon (1e-4));
                    CHECK (new_direction.y == Approx (direction.y).epsilon (1e-4));
                    distance = new_distance;
                }
            }
        }
    }
}

/* Copyright © 2022 Aaron Alef */
