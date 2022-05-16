#include <catch2/catch_test_macros.hpp>

#include "Random/random.h"

#include <array>
#include <vector>
#include <exception>

#include <iostream>

#define NUMBER_RNG 10

SCENARIO ("RNGs with same seed produce the same values") {
    HashedRNG random_seed;
    HashedRNG same_seed (random_seed.seed);

    WHEN ("The boundaries are real") {
        std::vector <Bounds> bounds = {
                {-100, -10},
                {-10, 0},
                {-10, 10},
                {0, 10},
                {10, 10},
                {0, 0},
                {1e-15, 2e-15},
                {1e15, 2e15}
        };

        for (auto & bound : bounds) {
            THEN ("Both RNGs produce the same results") {
                REQUIRE (same_seed.random (bound) == random_seed.random (bound));
            }
            THEN ("All random values are within bounds") {
                REQUIRE (bound.contains (random_seed.random (bound)));
            }
        }
    }

    WHEN ("lower bound is bigger than upper bound") {
        std::vector <Bounds> bounds = {
                {100, 10},
                {100, -100},
                {-100, -100000}
        };

        for (auto bound : bounds) {
            THEN ("RNGs produce values in that range") {
                REQUIRE (bound.contains (random_seed.random (bound)));
            }
            THEN ("RNGs produce the same results") {
                REQUIRE (same_seed.random (bound) == random_seed.random (bound));
            }
        }
    }
}

SCENARIO ("RNGs with different seeds provide different values") {
    std::array <HashedRNG, NUMBER_RNG> rngs;

    WHEN ("The boundaries are real") {
        WHEN ("Lower and upper boundary are not the same") {
            std::vector <Bounds> bounds = {
                    {-100, -10},
                    {-10, 0},
                    {-10, 10},
                    {0, 10},
                    {1e-15, 2e-15},
                    {1e15, 2e15}
            };

            THEN ("Each RNG produces a different valid value") {
                for (auto bound: bounds) {
                    for (int lower = 0; lower < NUMBER_RNG - 1; lower++) {
                        for (int upper = lower + 1; upper < NUMBER_RNG; upper++) {
                            REQUIRE (rngs[lower].random (bound) != rngs[upper].random (bound));
                            REQUIRE (bound.contains (rngs[lower].random (bound)));
                            REQUIRE (bound.contains (rngs[upper].random (bound)));
                        }
                    }
                }
            }
        }
        WHEN ("Lower and upper boundary are the same") {
            std::vector <Bounds> bounds = {
                    {-1e15, -1e15},
                    {0, 0},
                    {100, 100},
            };

            for (auto bound: bounds) {
                for (auto & one : rngs) {
                    for (auto & two : rngs) {
                        THEN ("both RNGs produce the same results") {
                            REQUIRE (one.random (bound) == two.random (bound));
                        }
                        THEN ("Each value is within range") {
                            REQUIRE (bound.contains (one.random (bound)));
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE ("Bounds initialised with one or more NaN values") {
    REQUIRE_THROWS_AS (Bounds (0, INFINITY), std::logic_error);
    REQUIRE_THROWS_AS (Bounds (0, NAN), std::logic_error);
    REQUIRE_THROWS_AS (Bounds (INFINITY, 0), std::logic_error);
    REQUIRE_THROWS_AS (Bounds (NAN, 0), std::logic_error);
    REQUIRE_THROWS_AS (Bounds (INFINITY, INFINITY), std::logic_error);
    REQUIRE_THROWS_AS (Bounds (INFINITY, NAN), std::logic_error);
    REQUIRE_THROWS_AS (Bounds (NAN, INFINITY), std::logic_error);
    REQUIRE_THROWS_AS (Bounds (NAN, NAN), std::logic_error);
}

/* Copyright © 2022 Aaron Alef */
