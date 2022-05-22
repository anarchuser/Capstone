#include <spqr.hpp>
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

    kj::Own <ShipHandleImpl> BackendImpl::registerShipCallback (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
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
        auto [iterator, success] = shipHandles.insert ({username, handle});
        KJ_ASSERT (success);

        broadcastSpaceship (spaceship);

        log ("Configure ShipHandle sent back to the client");
        auto sink = kj::heap <ShipHandleImpl> ();
        sink->setOnSendItem ([this, username] (Direction direction) {
            sendItemCallback (username, direction);
        });
        sink->setOnDone ([this, username] () {
            doneCallback (username);
        });

        return kj::mv (sink);
    }

    void BackendImpl::sendItemCallback (std::string const & ship, Direction direction) {
        KJ_REQUIRE (shipHandles.contains (ship), ship, "Cannot forward directions; ship handle not found");
        auto request = shipHandles.at (ship).sendItemRequest();
        direction.initialise (request.initItem().initDirection());
        // TODO: look for a better way
        // TODO: does this need detachment?
        request.send().then ([](...){});

        // TODO: forward direction to each connection
    }

    void BackendImpl::doneCallback (std::string username) {
        log ("Closing sinks from " + username);

        if (shipHandles.contains (username)) {
            auto & handle = shipHandles.at (username);
            // TODO: look for a better way
            // TODO: does this need detachment?
            handle.doneRequest().send().then ([](...){});
        } else {
            KJ_DLOG (WARNING, "No spaceship with username " + username + "found");
        }
    }

    void BackendImpl::broadcastSpaceship (Spaceship const & sender) {
        for (auto registrar = registrars.begin(); registrar != registrars.end(); registrar++) {
            auto request = registrar->registerShipRequest();
            sender.initialise (request.initSpaceship());
            request.send().detach ([&] (kj::Exception && e) {
                        KJ_DLOG (WARNING, e.getDescription());
                        KJ_DLOG (WARNING, "Removing registrar");
                registrars.erase (registrar);
            });
        }

        // TODO: distribute new ship to each connection
    }

    ::kj::Promise <void> BackendImpl::registerClient (RegisterClientContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasS2c_registrar());
        registrars.push_back (params.getS2c_registrar());

        auto results = context.initResults();
        auto registrar = kj::heap <ShipRegistrarImpl> ();
        registrar->setOnRegisterShip ([this] (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
            return registerShipCallback (spaceship, handle);
        });
        results.setC2s_registrar (kj::mv (registrar));

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