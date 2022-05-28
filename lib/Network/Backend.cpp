#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed, std::string id)
            : rng_seed {seed}
            , ID {id}
            {}

    BackendImpl::Client::Client (Backend::Registrar::Client && registrar, kj::Maybe <Backend::Synchro::Client> && synchro, Type type)
            : registrar {registrar}
            , synchro {synchro}
            , type {type}
            {}

    BackendImpl::Client::Client (Backend::Registrar::Client && registrar)
            : Client (std::forward <Backend::Registrar::Client> (registrar), {}, LOCAL)
            {}

    void BackendImpl::log (std::string const & msg) {
        std::ostringstream ss;
        ss << "Backend " << ID << " @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        debug_stdout (ss.str());
    }

    ::kj::Promise <void> BackendImpl::ping (PingContext context) {
        return kj::READY_NOW;
    }

    ::kj::Promise<void> BackendImpl::seed (SeedContext context) {
        log ("RNG Seed (" + std::to_string (rng_seed) + ") requested");
        context.initResults().setSeed (rng_seed);
        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::connect (ConnectContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        std::string const & remoteID = params.getId();
        KJ_REQUIRE (!clients.contains (remoteID), remoteID, "Client is already registered!");
        log ("Connect request received from client " + std::string (params.getId()));

        KJ_REQUIRE (params.hasRegistrar());
        clients.emplace (params.getId(), params.getRegistrar());
        log ("Number of clients connected: "s += std::to_string (clients.size()));

        auto results = context.getResults();
        results.setId (ID);
        auto registrar = kj::heap <RegistrarImpl>(remoteID);
        registrar->setOnRegisterShip ([this] (Spaceship const & ship, std::string const & id, Backend::ShipHandle::Client handle) {
            return registerShip (ship, id, handle);
        });
        results.setRegistrar (kj::mv (registrar));

        auto synchro = kj::heap <SynchroImpl>(remoteID);
        synchro->setOnConnect ([this] (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (id, synchro, registrar);
        });
        results.setSynchro (kj::mv (synchro));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        std::string const & remoteID = params.getId();
        log ("Join request received from " + remoteID);

        auto results = context.getResults();
        results.setId (ID);
        auto local = kj::heap <SynchroImpl> (remoteID);
        local->setOnConnect ([this] (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (id, synchro, registrar);
        });
        results.setLocal (kj::mv (local));

        /// Connect back to received synchro
        KJ_REQUIRE (params.hasRemote());
        auto connectRequest = params.getRemote().connectRequest();
        connectRequest.setId (ID);

        KJ_REQUIRE (results.hasId());
        auto synchro = kj::heap <SynchroImpl> (remoteID);
        synchro->setOnConnect ([this] (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (id, synchro, registrar);
        });
        connectRequest.setSynchro (kj::mv (synchro));

        auto registrar = kj::heap <RegistrarImpl> (remoteID);
        registrar->setOnRegisterShip ([this] (Spaceship ship, std::string const & id, Backend::ShipHandle::Client handle) {
            return registerShip (ship, id, handle);
        });
        connectRequest.setRegistrar (kj::mv (registrar));

        return connectRequest.send().then ([this, synchro = params.getRemote(), id = remoteID] (capnp::Response <Backend::Synchro::ConnectResults> results) mutable {
            KJ_REQUIRE (results.hasRegistrar());
            KJ_REQUIRE (results.getId() == id);
            KJ_REQUIRE (!clients.contains (id));
            clients.emplace (id, Client (results.getRegistrar(), synchro));
            log ("Number of clients connected: "s += std::to_string (clients.size()));
        });
    }

    ::kj::Own <RegistrarImpl> BackendImpl::connectCallback (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client remoteRegistrar) {
        log ("Received Synchro::connect request from " + id);
        clients.emplace (id, Client (std::move (remoteRegistrar), std::move (synchro)));
        log ("Number of clients connected: "s += std::to_string (clients.size()));

        auto registrar = kj::heap <RegistrarImpl>(id);
        registrar->setOnRegisterShip ([this] (Spaceship const & ship, std::string const & id, Backend::ShipHandle::Client handle) {
            return registerShip (ship, id, handle);
        });
        return registrar;
    }

    kj::Own <ShipSinkImpl> BackendImpl::registerShip (Spaceship const & ship, std::string const & id, Backend::ShipHandle::Client handle) {
        auto username = ship.username;
        KJ_REQUIRE (clients.contains (id), id, "Ship owner is not registered");
        auto & ships = clients.at (id).ships;
        KJ_REQUIRE (!ships.contains (username), id, username, "Client 'id' already contains a ship named 'username'");

        log ("Registering ship " + username + " from client " + id);
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
        sink->setOnSendItem ([this, username, id] (Direction const & direction) {
            return sendItemCallback (username, direction, id);
        });
        return sink;
    }

    kj::Promise <void> BackendImpl::broadcastSpaceship (Spaceship const & ship) {
        std::size_t i = 0;
        log ("Number of clients: " + std::to_string (clients.size()));
        for (auto & client : clients) {
            log ("Broadcast ship " + ship.username + " to client " + client.first);
            detach (distributeSpaceship (ship, client.second));
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::distributeSpaceship (Spaceship const & ship, Client & receiver) {
        auto & sender = ship.username;
        if (receiver.sinks.contains (sender)) return kj::READY_NOW;

        for (auto & client : clients) {
            // Figure out which client is the owner of this spaceship (NOTE: std::find could not handle this properly)
            // TODO: Don't use a loop for this
            auto & ships = client.second.ships;
            if (!ships.contains (sender)) continue;

            auto request = receiver.registrar.registerShipRequest ();
            ship.initialise (request.initShip ());
            request.setHandle (ships.at (sender));
            auto promise = request.send ();
            return promise.then ([this, sender, & receiver, client = client.first]
                    (capnp::Response<Backend::Registrar::RegisterShipResults> results) {
                        // Check again if nothing's changed
                        if (receiver.sinks.contains (sender)) return;

                        receiver.sinks.insert_or_assign (sender, results.getSink ());
                    });
        }
        KJ_FAIL_REQUIRE ("No handle for the given spaceship exists", ship.username);
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::doneCallback (std::string const & username) {
        log ("Disconnecting " + username);
        for (auto & client : clients) {
            auto & sinks = client.second.sinks;
            if (!sinks.contains (username)) continue;
            sinks.at (username).doneRequest().send().detach ([](...){});  // Ignore errors on done
            sinks.erase (username);

            auto & ships = client.second.ships;
            ships.erase (username);
            if (ships.empty()) clients.erase (client.first);
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::sendItemCallback (std::string const & username, Direction const & direction, std::string const & id) {
        KJ_REQUIRE (clients.contains (id), id, username, "Received item from unregistered client");
        auto & sender = clients.at (id);
        KJ_REQUIRE (sender.ships.contains (username), id, username, "Client does not own ship 'username'");

        /// Case 1: Item got sent from a local client -> Distribute to everyone
        if (sender.type == Client::LOCAL) {
            for (auto & client : clients) {
                /* Send Item from local ship to sink of same name of every client.
                 * If that client does not have a sink with that name yet, create it
                 */

                sendItemToClient (username, direction, client.second).detach (
                        [this, id = client.first] (kj::Exception && e) { disconnect (id); });
            }
        }
        /// Case 2: Item got sent from a remote client -> Distribute to our own ship only
        else {
            for (auto & client : clients) {
                // Do not send the item to remote clients, as they've received the item directly already
                if (client.second.type == Client::REMOTE) continue;

                // Send the item to the corresponding spaceship of our own local clients, or create it if needed
                sendItemToClient (username, direction, client.second).detach (
                        [this, id = client.first, username = username] (kj::Exception && e) {
                            KJ_DLOG (WARNING, id, username, e.getDescription ());
                            if (clients.contains (id)) clients.at (id).sinks.erase (username);
                        });
            }
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::sendItemToClient (std::string const & username, Direction const & direction, Client & receiver) {
        auto & sinks = receiver.sinks;

        if (!sinks.contains (username)) {
            log ("Missing sink to ship " + username);

            for (auto & client : clients) {
                // Find the client that owns the ship in question
                auto & ships = client.second.ships;
                if (!ships.contains (username)) continue;

                // The current client owns the ship; proceed
                return ships.at (username).getShipRequest().send()
                        .then ([&] (capnp::Response <Backend::ShipHandle::GetShipResults> results) {
                            Spaceship ship (results.getShip());
                            KJ_DASSERT (ship.username == username);  // TODO: username should not be mandatory here
                            return distributeSpaceship (ship, receiver);
                        });
            }
        }
        auto request = sinks.at (username).sendItemRequest();
        direction.initialise (request.initItem().initDirection());
        return request.send().ignoreResult()
            .catch_ ([this, & sinks, & username] (kj::Exception && e) {
            log ("Connection lost to " + username);
            sinks.erase (username);
            for (auto & client : clients) {
                if (client.second.type == Client::LOCAL) client.second.ships.erase (username);
            }
        })
        ;
    }

    void BackendImpl::disconnect (std::string const & id) {
        if (!clients.contains (id)) return;
        log ("Disconnecting client " + id);

        auto & client = clients.at (id);
        for (auto & sink : client.sinks) {
            sink.second.doneRequest().send().detach ([](...){});
        }
        clients.erase (id);
    }
}

/* Copyright © 2022 Aaron Alef */