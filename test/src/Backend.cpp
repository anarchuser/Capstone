#include <catch2/catch_test_macros.hpp>

#include "Backend/Backend.h"
#include <exception>

#include "Random/random.h"

#define RANDOM_SEED generateSeed()

SCENARIO ("The Synchro implementation handles requests properly") {
    GIVEN ("No running server") {
        capnp::EzRpcClient client ("localhost:44444");
        auto & waitScope = client.getWaitScope();

        WHEN ("I connect to a non-existent address") {
            auto main = client.getMain <Synchro>();
            auto request = main.pingRequest();

            THEN ("It throws an error") {
                REQUIRE_THROWS_AS (request.send().wait (waitScope), std::exception);
            }
        }
    }

    GIVEN ("a Synchro instance on a different thread") {
        auto const seed = RANDOM_SEED;
        kt::Backend backend (seed, "*");
        auto port = backend.getPort();

        capnp::EzRpcClient client ("localhost", port);
        auto main = client.getMain <Synchro>();
        auto & waitScope = client.getWaitScope();

        WHEN ("I send a ping") {
            auto request = main.pingRequest();

            THEN ("It resolves without fail") {
                REQUIRE_NOTHROW (request.send().wait (waitScope));
            }
        }
        WHEN ("I request the RNG seed") {
            auto request = main.seedRequest();

            THEN ("I get back the expected seed without fail") {
                REQUIRE (request.send().wait (waitScope).getSeed() == seed);
            }
        }
    }
}

/* Copyright © 2022 Aaron Alef */
