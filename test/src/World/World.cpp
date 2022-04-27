#include <catch2/catch_test_macros.hpp>

#include "Game/World/World.h"

#define WINDOW_SIZE 100
#define PHYS_SCALE  0.1

SCENARIO ("World handles objects as expected") {
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
    }
}

/* Copyright © 2022 Aaron Alef */
