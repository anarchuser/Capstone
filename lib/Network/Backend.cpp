#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed)
            : rng_seed {seed}
            {}

    BackendImpl::Registrar::Registrar (Backend::Registrar::Client registrar): registrar {registrar} {}

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
        clients.emplace_back (params.getRegistrar());
        log ("Number of clients connected: "s += std::to_string (clients.size()));

        auto results = context.getResults();
        auto registrar = kj::heap <RegistrarImpl>();
        registrar->setOnRegisterShip ([this] (Spaceship ship, Backend::ShipHandle::Client handle) {
            return registerShip (ship, handle);
        });
        results.setRegistrar (kj::mv (registrar));

        auto synchro = kj::heap <SynchroImpl>();
        synchro->setOnConnect ([this] (Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (synchro, registrar);
        });
        results.setSynchro (kj::mv (synchro));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        log ("Join request received");

        auto params = context.getParams();
        KJ_REQUIRE (params.hasRemote());
        synchros.emplace_back (params.getRemote());
        log ("Number of synchros connected: "s += std::to_string (synchros.size()));

        auto result = kj::heap <SynchroImpl> ();
        result->setOnConnect ([this] (Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (synchro, registrar);
        });
        context.getResults().setLocal (kj::mv (result));

        /// Connect back to received synchro
        auto connectRequest = params.getRemote().connectRequest();
        auto synchro = kj::heap <SynchroImpl> ();
        synchro->setOnConnect ([this] (Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (synchro, registrar);
        });
        connectRequest.setSynchro (kj::mv (synchro));
        auto registrar = kj::heap <RegistrarImpl> ();
        registrar->setOnRegisterShip ([this] (Spaceship ship, Backend::ShipHandle::Client handle) {
            return registerShip (ship, handle);
        });
        connectRequest.setRegistrar (kj::mv (registrar));
        return connectRequest.send().then ([this] (capnp::Response <Backend::Synchro::ConnectResults> results) {
            KJ_REQUIRE (results.hasRegistrar());
            clients.emplace_back (results.getRegistrar());
            log ("Number of clients connected: "s += std::to_string (clients.size()));
        });
    }

    ::kj::Own <RegistrarImpl> BackendImpl::connectCallback (Backend::Synchro::Client synchro, Backend::Registrar::Client remoteRegistrar) {
        clients.emplace_back (remoteRegistrar);
        log ("Number of clients connected: "s += std::to_string (clients.size()));

        synchros.emplace_back (synchro);
        log ("Number of synchros connected: "s += std::to_string (synchros.size()));

        auto registrar = kj::heap <RegistrarImpl>();
        registrar->setOnRegisterShip ([this] (Spaceship ship, Backend::ShipHandle::Client handle) {
            return registerShip (ship, handle);
        });
        return registrar;
    }

    kj::Own <ShipSinkImpl> BackendImpl::registerShip (Spaceship const & ship, Backend::ShipHandle::Client handle) {
        auto username = ship.username;
        KJ_REQUIRE (!ships.contains (username), username, "Name duplication detected");

        log ("Registering ship " + username);
        log ("Position: / "  + std::to_string (ship.position[0]) + " | " + std::to_string (ship.position[1]) + " \\");
        log ("Velocity: \\ " + std::to_string (ship.velocity[0]) + " | " + std::to_string (ship.velocity[1]) + " /");
        log ("Health: " + std::to_string (ship.health));

        auto [iterator, success] = ships.emplace (username, handle);
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
        std::size_t i = 0;
        for (auto & client : clients) {
            log ("Broadcast ship " + ship.username + + " to client [" + std::to_string (++i) + "/" + std::to_string (clients.size()) + "]");
            detach (distributeSpaceship (ship, client));
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::distributeSpaceship (Spaceship const & ship, Registrar & receiver) {
        auto & sender = ship.username;
        if (receiver.sinks.contains (sender)) {
            return kj::READY_NOW;
        }
        KJ_REQUIRE (ships.contains (sender));

        auto request = receiver.registrar.registerShipRequest();
        ship.initialise (request.initShip());
        request.setHandle (ships.at (sender));
        auto promise = request.send();
        return promise.then ([this, sender, & receiver] (capnp::Response <Backend::Registrar::RegisterShipResults> results) {
            receiver.sinks.emplace (sender, results.getSink());
        });
    }
    kj::Promise <void> BackendImpl::doneCallback (std::string const & username) {
        log ("Disconnecting " + username);
        if (!ships.contains (username)) {
            return kj::READY_NOW;
        }
        for (auto & client : clients) {
            auto & sinks = client.sinks;
            if (!sinks.contains (username)) continue;
            detach (sinks.at (username).doneRequest().send().ignoreResult());
            sinks.erase (username);
        }
        ships.erase (username);
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::sendItemCallback (std::string const & username, Direction const & direction) {
        if (!ships.contains (username)) {
            log ("Received sendItem request for " + username + ", for which no record exists (anymore?)");
            return kj::READY_NOW;
        }
        KJ_ASSERT (ships.contains (username));
        for (auto & client : clients) {
            detach (sendItemToClient (username, direction, client));
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::sendItemToClient (std::string const & username, Direction const & direction, Registrar & receiver) {
        auto & sinks = receiver.sinks;
        if (!sinks.contains (username)) {
            log ("Missing sink to ship " + username);
            KJ_REQUIRE (ships.contains (username));
            return ships.at (username).getShipRequest().send()
                    .then ([&] (capnp::Response <Backend::ShipHandle::GetShipResults> results) {
                        Spaceship ship (results.getShip());
                        KJ_ASSERT (ship.username == username);
                        return distributeSpaceship (ship, receiver);
                    });
        }
        auto request = sinks.at (username).sendItemRequest();
        direction.initialise (request.initItem().initDirection());
        return request.send().ignoreResult().catch_ ([this, & sinks, & username] (kj::Exception && e) {
            KJ_DLOG (WARNING, e.getDescription());
            sinks.erase (username);
        });
    }
}

/* Copyright © 2022 Aaron Alef */