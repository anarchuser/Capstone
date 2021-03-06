#include <catch2/catch_test_macros.hpp>

#include "World/World.h"
#include "Planet/Planet.h"
#include "Random/random.h"

#define WINDOW_SIZE 100
#define PHYS_SCALE  0.1

#define TEST_RANDOM_SEED 1

SCENARIO ("World handles objects as expected") {
    auto rng = RNG (TEST_RANDOM_SEED);

    GIVEN ("An empty world") {
        Stage::instance = new Stage;
        getStage()->setSize (WINDOW_SIZE, WINDOW_SIZE);

        auto world = kt::World (nullptr, {WINDOW_SIZE * PHYS_SCALE, WINDOW_SIZE * PHYS_SCALE});

        THEN ("The physical world starts as default") {
            auto & physical_world = world.world;

            REQUIRE (physical_world.GetBodyCount() == 0);
            REQUIRE (physical_world.GetGravity() == b2Vec2_zero);
        }

        WHEN ("physics and game sizes are used") {
            auto phy_size = world.world_size;
            auto win_size = world.getSize();

            THEN ("They can be converted back and forth") {
                REQUIRE (win_size == getStage ()->getSize ());
                REQUIRE (world.convert (win_size) == phy_size);
                REQUIRE (world.convert (phy_size) == win_size);
                REQUIRE (world.convert (world.convert (win_size)) == win_size);
                REQUIRE (world.convert (world.convert (phy_size)) == phy_size);
            }
        }

        WHEN ("Objects are added within range") {
            auto pos = Vector2 (25, 50);
            auto phys_pos = b2Vec2 (PHYS_SCALE * 25, PHYS_SCALE * 50);
            kt::spPlanet planet = new kt::Planet (world, nullptr, pos);
            world.addChild (planet);

            THEN ("They are appropriately scaled in the world") {
                auto & child = * world.getLastChild ();
                REQUIRE (child.getPosition () == pos);
                REQUIRE (world.convert (child.getPosition ()) == phys_pos);

                auto * body = (b2Body *) child.getUserData ();
                REQUIRE (body->GetPosition () == phys_pos);
                REQUIRE (world.convert (body->GetPosition ()) == pos);
            }
        }

        WHEN ("Objects are randomly added within range") {
            auto pos = Vector2 (rng.random ({0, WINDOW_SIZE}), rng.random ({0, WINDOW_SIZE}));
            auto phys_pos = b2Vec2 (pos.x * PHYS_SCALE, pos.y * PHYS_SCALE);
            kt::spPlanet planet = new kt::Planet (world, nullptr, pos);
            world.addChild (planet);

            THEN ("They are appropriately scaled in the world") {
                auto & child = * world.getLastChild ();

                REQUIRE (child.getPosition () == pos);
                REQUIRE (world.convert (child.getPosition ()) == phys_pos);

                auto * body = (b2Body *) child.getUserData ();
                REQUIRE (body->GetPosition() == phys_pos);
                REQUIRE (world.convert (body->GetPosition ()) == pos);
            }
        }

        WHEN ("Objects are added outside range") {
            auto pos = Vector2 (-200, 500);
            auto phys_pos = b2Vec2 (PHYS_SCALE * -200, PHYS_SCALE * 500);

            REQUIRE (world.convert (pos) == phys_pos);
            REQUIRE (world.convert (phys_pos) == pos);

            kt::spPlanet planet = new kt::Planet (world, nullptr, pos);
            world.addChild (planet);

            THEN ("They are appropriately scaled in the world") {
                auto & child = * world.getLastChild ();
                REQUIRE (child.getPosition () != pos);
                REQUIRE (world.convert (child.getPosition ()) != phys_pos);
                REQUIRE (world.convert (child.getPosition ()) == world.wrap (phys_pos));

                auto * body = (b2Body *) child.getUserData ();
                REQUIRE (body->GetPosition () != phys_pos);
                REQUIRE (world.convert (body->GetPosition ()) != pos);
                REQUIRE (body->GetPosition () == world.wrap (phys_pos));
            }
        }

        WHEN ("Objects are randomly added outside range") {
            auto pos = Vector2 (rng.random ({-1e8, 0}), rng.random ({WINDOW_SIZE, 1e8}));
            auto phys_pos = b2Vec2 (pos.x * PHYS_SCALE, pos.y * PHYS_SCALE);

            REQUIRE (world.convert (pos) == phys_pos);
            REQUIRE (world.convert (phys_pos) == pos);

            kt::spPlanet planet = new kt::Planet (world, nullptr, pos);
            world.addChild (planet);

            THEN ("They are appropriately scaled in the world") {
                auto & child = * world.getLastChild ();
                REQUIRE (child.getPosition () != pos);
                REQUIRE (world.convert (child.getPosition ()) != phys_pos);
                REQUIRE (world.convert (child.getPosition ()) == world.wrap (phys_pos));

                auto * body = (b2Body *) child.getUserData ();
                REQUIRE (body->GetPosition () != phys_pos);
                REQUIRE (world.convert (body->GetPosition ()) != pos);
                REQUIRE (body->GetPosition () == world.wrap (phys_pos));
            }
        }
    }
}

/* Copyright ?? 2022 Aaron Alef */
