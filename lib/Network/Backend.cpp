#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed)
            : rng_seed {seed}
            {}

    BackendImpl::RegisteredShip::RegisteredShip (Backend::ShipHandle::Client handle): handle {handle} {}

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

    ::kj::Promise <void> BackendImpl::connect (ConnectContext context) {
        log ("Connect request received");

        auto params = context.getParams();
        KJ_REQUIRE (params.hasRegistrar());
        clients.push_back (params.getRegistrar());
        log ("Number of clients connected: "s += std::to_string (clients.size()));

        auto results = context.getResults();
        auto registrar = kj::heap <RegistrarImpl>();
        registrar->setOnRegisterShip ([this] (Spaceship ship, Backend::ShipHandle::Client handle) {
            return registerShip (ship, handle);
        });
        results.setRegistrar (kj::mv (registrar));

        auto synchro = kj::heap <SynchroImpl>();
        results.setSynchro (kj::mv (synchro));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        log ("Join request received");

        auto params = context.getParams();
        KJ_REQUIRE (params.hasRemote());
        synchros.push_back (params.getRemote());
        log ("Number of synchros connected: "s += std::to_string (synchros.size()));

        auto results = context.getResults();
        results.setLocal (kj::heap <SynchroImpl> ());

        return kj::READY_NOW;
    }

    kj::Own <ShipSinkImpl> BackendImpl::registerShip (Spaceship const & ship, Backend::ShipHandle::Client handle) {
        auto username = ship.username;
        if (ships.contains (username)) {
            log ("username " + username + " existed already");
            ships.erase (username);
        }
        KJ_REQUIRE (!ships.contains (username), username, "Name duplication detected");

        log ("Registering ship " + username);
        log ("Position: / "  + std::to_string (ship.position[0]) + " | " + std::to_string (ship.position[1]) + " \\");
        log ("Velocity: \\ " + std::to_string (ship.velocity[0]) + " | " + std::to_string (ship.velocity[1]) + " /");
        log ("Health: " + std::to_string (ship.health));

        auto [iterator, success] = ships.emplace (username, RegisteredShip (handle));
        KJ_ASSERT (success);

        detach (broadcastSpaceship (ship));

        // Prepare ShipSink
        auto sink = kj::heap <ShipSinkImpl>();
        sink->setOnDone ([this, username] () {
            return doneCallback (username);
        });
        sink->setOnSendItem ([this, username] (Direction const & direction) {
            return sendItemCallback (username, direction);
        });
        return sink;
    }

    kj::Promise <void> BackendImpl::broadcastSpaceship (Spaceship const & ship) {
        kj::Promise <void> promise = kj::READY_NOW;
        for (auto & registeredShip : ships) {
            promise.exclusiveJoin (distributeSpaceship (ship, registeredShip.first));
        }
        return promise;
    }
    kj::Promise <void> BackendImpl::distributeSpaceship (Spaceship const & ship, std::string const & receiver) {
        auto sender = ship.username;
        log ("Distributing ship from " + sender + " to " + receiver);
        KJ_REQUIRE (ships.contains (sender));
        KJ_REQUIRE (ships.contains (receiver));

        auto request = clients.front().registerShipRequest ();
        ship.initialise (request.initShip());
        return request.send().then ([&] (capnp::Response <Backend::Registrar::RegisterShipResults> results) {
                    log ("Received sink from " + sender);
                    ships.at (receiver).sinks.emplace (sender, results.getSink());
                });
    }

    kj::Promise <void> BackendImpl::doneCallback (std::string const & username) {
        log ("Disconnecting " + username);
        if (!ships.contains (username)) {
            log ("No ship found");
            return kj::READY_NOW;
        }
        auto & ship = ships.at (username);
        kj::Promise <void> promise = kj::READY_NOW;
        for (auto & sink : ship.sinks) {
            promise.exclusiveJoin (sink.second.doneRequest().send().ignoreResult());
            ship.sinks.erase (sink.first);
        }
        ships.erase (username);
        return promise;
    }

    kj::Promise <void> BackendImpl::sendItemCallback (std::string const & username, Direction const & direction) {
        log ("Processing item sent by " + username);

        if (!ships.contains (username)) {
            log ("Received sendItem request for " + username + ", for which no record exists (anymore?)");
            return kj::READY_NOW;
        }
        KJ_ASSERT (ships.contains (username));
        kj::Promise <void> promise = kj::READY_NOW;
        for (auto & ship : ships) {
            auto & sinks = ship.second.sinks;
            if (!sinks.contains (username)) {
                log ("Missing connection from " + username + " to " + ship.first);
                return ships.at (username).handle.getShipRequest().send()
                        .then ([&] (capnp::Response <Backend::ShipHandle::GetShipResults> results) {
                            return distributeSpaceship (Spaceship (results.getShip()), ship.first);
                        });
            }
            auto request = sinks.at (username).sendItemRequest();
            direction.initialise (request.initItem().initDirection());
            promise.exclusiveJoin (request.send().ignoreResult());
        }
        return promise;
    }
}

/* Copyright © 2022 Aaron Alef */