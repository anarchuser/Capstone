#include <catch2/catch_test_macros.hpp>

#include "Game/World/World.h"

#define WINDOW_SIZE 100
#define PHYS_SCALE  0.1

SCENARIO ("World handles objects as expected") {
    GIVEN ("An empty world") {
        Stage::instance = new Stage;
        getStage()->setSize (WINDOW_SIZE, WINDOW_SIZE);

        auto world = kt::World (nullptr, {WINDOW_SIZE * PHYS_SCALE, WINDOW_SIZE * PHYS_SCALE});

        THEN ("The physical world starts empty") {
            auto & physical_world = world.world;

            REQUIRE (physical_world.GetBodyCount() == 0);
            REQUIRE (physical_world.GetGravity() == b2Vec2_zero);
        }
    }
}

/* Copyright © 2022 Aaron Alef */
