#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed): rng_seed {seed} {}

    void BackendImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Backend @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    ::kj::Promise <void> BackendImpl::ping (PingContext context) {
        log ("Ping");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> BackendImpl::seed (SeedContext context) {
        log ("RNG Seed (" + std::to_string (rng_seed) + ") requested");
        context.initResults().setSeed (rng_seed);
        return kj::READY_NOW;
    }

    kj::Own <ItemSinkImpl> BackendImpl::registerShipCallback (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
        // If username exists already, replace:
        std::string const & username = spaceship.username;
        if (shipHandles.contains (username)) {
            doneCallback (username);
        }
        KJ_REQUIRE (! shipHandles.contains (username), username, "Username already in use");

        log (std::string ("Join request received from user ") + username);
        log (std::string ("Position: ( ")
             + std::to_string (spaceship.position[0]) + " | "
             + std::to_string (spaceship.position[1]) + " )");
        log (std::string ("Velocity: ( ")
                + std::to_string (spaceship.velocity[0]) + " | "
                + std::to_string (spaceship.velocity[1]) + " )");
        log (std::string ("Angle: " + std::to_string (spaceship.angle)));
        log (std::string ("Health: " + std::to_string (spaceship.health)));

        // Store connection details (callback handles and 'new ship' callback)
        auto [iterator, success] = shipHandles.insert ({username, ShipHandle (std::move (handle))});
        KJ_ASSERT (success);
        broadcastSpaceship (spaceship);

        // Configure ItemSink sent back to client
        log ("Configure ItemSink result");
        auto sink = kj::heap <ItemSinkImpl> ();
        sink->setOnSendItem ([this, username] (Direction direction) {
            sendItemCallback (username, direction);
        });
        sink->setOnDone ([this, username] () {
            doneCallback (username);
        });

        return kj::mv (sink);
    }

    void BackendImpl::sendItemCallback (std::string const & sender, Direction direction) {
        std::for_each (shipHandles.begin(), shipHandles.end(),
                       [&] (std::pair <std::string const, ShipHandle> & pair) {
            auto & receiver = pair.first;
            auto & sinks = pair.second.itemSinks;

            if (! sinks.contains (sender)) {
                detach(shipHandles.at (sender).shipHandle.getSpaceshipRequest().send().then (
                        [&] (capnp::Response <Backend::ShipHandle::GetSpaceshipResults> response) {
                    distributeSpaceship (Spaceship (response.getSpaceship()), receiver);
                    sendItemCallback (sender, direction);
                }));
                return;
            }

            KJ_REQUIRE (sinks.contains (sender));
            auto request = sinks.at (sender).sendItemRequest();
            auto request_direction = request.initItem().initDirection();

            request_direction.setAccelerate (direction.accelerate);
            request_direction.setDecelerate (direction.decelerate);
            request_direction.setRotateLeft (direction.rotateLeft);
            request_direction.setRotateRight (direction.rotateRight);

            request.send();
        });
    }
    void BackendImpl::doneCallback (std::string username) {
        log ("Closing sinks from " + username);

        shipHandles.erase (username);
        std::for_each (shipHandles.begin(), shipHandles.end(),
                       [&] (std::pair <std::string const, ShipHandle> & pair) {
            auto & sinks = pair.second.itemSinks;
            if (sinks.contains (username)) {
                sinks.at (username).doneRequest ().send ().then ([](...){});
                sinks.erase (username);
            }
        });
    }

    void BackendImpl::distributeSpaceship (Spaceship const & sender, std::string const & receiver) {
        log ("Relay directions from " + sender.username += " to " + receiver);

        KJ_REQUIRE (shipHandles.contains (sender.username));
        KJ_REQUIRE (shipHandles.contains (receiver));

        if (shipHandles.at (receiver).itemSinks.contains (sender.username)) {
            log ("Connection exists already");
            return;
        }

        auto & shipHandle = shipHandles.at (receiver).shipHandle;
        auto & sinks = shipHandles.at (receiver).itemSinks;
        for (auto & registrar : registrars) {
            auto request = registrar.registerShipRequest ();
            sender.initialise (request.initSpaceship());
            sinks.emplace (sender.username, request.send().getSink());
        }
    }

    void BackendImpl::broadcastSpaceship (Spaceship const & sender) {
        // TODO: distribute sender to every registrar
    }

    ::kj::Promise <void> BackendImpl::registerClient (RegisterClientContext context) {
        auto results = context.initResults();
        auto registrar = kj::heap <ShipRegistrarImpl> ();

        registrar->setOnRegisterShip ([this] (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
            return registerShipCallback (spaceship, handle);
        });

        results.setRegistrar (kj::mv (registrar));

        return kj::READY_NOW;
    }

    ::kj::Promise<void> BackendImpl::connect (Backend::Server::ConnectContext context) {
        return kj::READY_NOW;
    }

    ::kj::Promise<void> BackendImpl::requestConnect (Backend::Server::RequestConnectContext context) {
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */