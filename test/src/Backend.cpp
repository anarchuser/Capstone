#include <catch2/catch_test_macros.hpp>

#include "Backend/Backend.h"
#include <exception>

#include "Random/random.h"

#include "env.h"

#define RANDOM_SEED generateSeed()

#define USERNAME hostname()
#define HEALTH_VALUE 42

SCENARIO ("A backend handles basic requests") {
    GIVEN ("A backend running on a local port") {
        auto const seed = RANDOM_SEED;
        kt::Backend backend (seed, "*");
        auto port = backend.getPort ();

        capnp::EzRpcClient client ("*", port);
        auto main = client.getMain<::Backend> ();
        auto & waitScope = client.getWaitScope ();

        WHEN ("I send a ping") {
            auto request = main.pingRequest ();

            THEN ("It resolves without fail") {
                REQUIRE_NOTHROW (request.send ().wait (waitScope));
            }
        }
        WHEN ("I request the RNG seed") {
            auto request = main.seedRequest ();

            THEN ("I get back the expected seed without fail") {
                logs::messageln ("Before seed request");
                REQUIRE (request.send ().wait (waitScope).getSeed () == seed);
                logs::messageln ("After seed request");
            }
        }
        WHEN ("I register as client") {
            auto registerClientRequest = main.connectRequest();
            registerClientRequest.setId (USERNAME);

            auto registrar = kj::heap <cg::RegistrarImpl> ();
            registrar->setOnRegisterShip ([id = USERNAME] (cg::Spaceship const & ship, cg::ShipHandle_t const & handle) {
                CHECK (id == USERNAME);

                CHECK (ship.health == HEALTH_VALUE);

                auto sink = kj::heap <cg::ShipSinkImpl> ();
                sink->setOnDone ([]() -> kj::Promise <void> { return kj::READY_NOW; });
                sink->setOnSendItem ([] (cg::Item const & item) -> kj::Promise <void> {
                    auto const & [time, dir, data] = item;

                    CHECK (dir.accelerate  == -1);
                    CHECK (dir.decelerate  ==  1);
                    CHECK (dir.rotateLeft  == -1);
                    CHECK (dir.rotateRight == -1);

                    CHECK (data.username == USERNAME);

                    return kj::READY_NOW;
                });
                return kj::mv (sink);
            });
            registerClientRequest.setRegistrar (kj::mv (registrar));

            THEN ("I get back a proper result without fail") {
                auto registerClientResult = registerClientRequest.send().wait (waitScope);

                REQUIRE (registerClientResult.hasSynchro());
                auto synchro = registerClientResult.getSynchro();
                // TODO: test connection to synchro

                REQUIRE (registerClientResult.hasRegistrar());
                auto registrar = registerClientResult.getRegistrar();

                WHEN ("I send a registerShip request back to the received registrar") {
                    auto registerShipRequest = registrar.registerShipRequest();
                    registerShipRequest.initShip().setHealth (HEALTH_VALUE);
                    registerShipRequest.getShip().setUsername (USERNAME);
                    auto handle = kj::heap <cg::ShipHandleImpl>();
                    handle->setOnGetShip ([]() { return cg::Spaceship (USERNAME, {}, {}, {}, HEALTH_VALUE); });
                    registerShipRequest.setHandle (kj::mv (handle));

                    THEN ("It returns a valid ShipHandle") {
                        auto registerShipResult = registerShipRequest.send().wait (waitScope);
                        REQUIRE (registerShipResult.hasSink());

                        auto sink = registerShipResult.getSink();

                        WHEN ("I send a sendItem request") {
                            auto sendItemRequest = sink.sendItemRequest ();
                            sendItemRequest.initItem().initDirection().setDecelerate (1);
                            sendItemRequest.getItem().initSpaceship().setUsername (USERNAME);

                            THEN ("It solves without fail") {
                                REQUIRE_NOTHROW (sendItemRequest.send().wait (waitScope));
                            }
                        }
                        WHEN ("I send a done request back") {
                            auto doneRequest = sink.doneRequest();

                            THEN ("It resolves without fail") {
                                REQUIRE_NOTHROW (doneRequest.send().wait (waitScope));
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE ("Ping checks whether the backend is running on a local port ") {
    SECTION ("Ping without active server") {
        REQUIRE_FALSE (kt::Backend::ping ("*", SERVER_PORT));
    }

    SECTION ("Ping with active server") {
        kt::Backend backend (RANDOM_SEED, SERVER_ADDRESS);

        REQUIRE (kt::Backend::ping ("*", backend.getPort()));
    }
}

/* Copyright © 2022 Aaron Alef */
