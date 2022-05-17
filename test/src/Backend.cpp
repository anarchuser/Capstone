#include <catch2/catch_test_macros.hpp>

#include "Backend/Backend.h"
#include <exception>

#include "Random/random.h"

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
        auto const seed = generateSeed();
        kt::Backend backend (seed, "*");

        auto port = backend.getPort();

        WHEN ("I send a ping") {

        }
    }
}

/* Copyright © 2022 Aaron Alef */
