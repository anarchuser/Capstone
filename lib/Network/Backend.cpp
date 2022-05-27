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
        ss << "Backend @" << this << ": '" << msg << "'";
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
        log ("Connect request received from client " + std::string (params.getId()));

        KJ_REQUIRE (params.hasRegistrar());
        clients.emplace (params.getId(), params.getRegistrar());
        log ("Number of clients connected: "s += std::to_string (clients.size()));

        auto results = context.getResults();
        results.setId (ID);
        auto registrar = kj::heap <RegistrarImpl>();
        registrar->setOnRegisterShip ([this] (Spaceship ship, Backend::ShipHandle::Client handle) {
            return registerShip (ship, ID, handle);
        });
        results.setRegistrar (kj::mv (registrar));

        auto synchro = kj::heap <SynchroImpl>(ID);
        synchro->setOnConnect ([this] (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (id, synchro, registrar);
        });
        results.setSynchro (kj::mv (synchro));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        log ("Join request received from " + std::string (params.getId()));

        auto results = context.getResults();
        results.setId (ID);
        auto local = kj::heap <SynchroImpl> (ID);
        local->setOnConnect ([this] (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (id, synchro, registrar);
        });
        results.setLocal (kj::mv (local));

        /// Connect back to received synchro
        KJ_REQUIRE (params.hasRemote());
        auto connectRequest = params.getRemote().connectRequest();
        connectRequest.setId (ID);
        auto synchro = kj::heap <SynchroImpl> (ID);
        synchro->setOnConnect ([this] (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client registrar) {
            return connectCallback (id, synchro, registrar);
        });
        connectRequest.setSynchro (kj::mv (synchro));
        auto registrar = kj::heap <RegistrarImpl> ();
        registrar->setOnRegisterShip ([this] (Spaceship ship, Backend::ShipHandle::Client handle) {
            return registerShip (ship, ID, handle);
        });
        connectRequest.setRegistrar (kj::mv (registrar));
        return connectRequest.send().then ([this, synchro = params.getRemote()] (capnp::Response <Backend::Synchro::ConnectResults> results) mutable {
            KJ_REQUIRE (results.hasRegistrar());
            KJ_REQUIRE (results.hasId());
            clients.emplace (results.getId(), Client (results.getRegistrar(), synchro));
            log ("Number of clients connected: "s += std::to_string (clients.size()));
        });
    }

    ::kj::Own <RegistrarImpl> BackendImpl::connectCallback (std::string const & id, Backend::Synchro::Client synchro, Backend::Registrar::Client remoteRegistrar) {
        log ("Received Synchro::connect request from " + id);
        clients.emplace (id, Client (std::move (remoteRegistrar), std::move (synchro)));
        log ("Number of clients connected: "s += std::to_string (clients.size()));

        auto registrar = kj::heap <RegistrarImpl>();
        registrar->setOnRegisterShip ([this] (Spaceship ship, Backend::ShipHandle::Client handle) {
            return registerShip (ship, ID, handle);
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
        for (auto & client : clients) {
            log ("Broadcast ship " + ship.username + + " to client [" + std::to_string (++i) + "/" + std::to_string (clients.size()) + "]");
            detach (distributeSpaceship (ship, client.second));
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::distributeSpaceship (Spaceship const & ship, Client & receiver) {
        auto & sender = ship.username;
        if (receiver.sinks.contains (sender)) return kj::READY_NOW;

        auto & ships = receiver.ships;
        KJ_REQUIRE (ships.contains (sender));

        auto request = receiver.registrar.registerShipRequest();
        ship.initialise (request.initShip());
        request.setHandle (ships.at (sender));
        auto promise = request.send();
        return promise.then ([this, sender, & receiver] (capnp::Response <Backend::Registrar::RegisterShipResults> results) {
            // Check again if nothing's changed
            if (receiver.sinks.contains (sender)) return;
            receiver.sinks.insert_or_assign (sender, results.getSink());
        });
    }
    kj::Promise <void> BackendImpl::doneCallback (std::string const & username) {
        log ("Disconnecting " + username);
        for (auto & client : clients) {
            auto & ships = client.second.ships;
            if (!ships.contains (username)) continue;
            ships.erase (username);

            auto & sinks = client.second.sinks;
            if (!sinks.contains (username)) continue;
            sinks.at (username).doneRequest().send().ignoreResult().detach ([](...){});  // Ignore errors on done
            sinks.erase (username);
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
                        [this, id = client.first, username = username] (kj::Exception && e) {
                            KJ_DLOG (WARNING, id, username, e.getDescription ());
                            if (clients.contains (id)) clients.at (id).sinks.erase (username);
                        });
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

            std::cout << "| Clients total: " << clients.size() << std::endl;
            for (auto & client : clients) {
                std::cout << "|-- Client '" << client.first << std::endl;
                std::cout << "|---- Sinks total: " << client.second.sinks.size() << std::endl;
                for (auto & sink : client.second.sinks) {
                    std::cout << "|------ Ship name: " << sink.first << std::endl;
                }
                std::cout << "|\n|---- Ships total: " << client.second.ships.size() << std::endl;
                for (auto & ship : client.second.ships) {
                    std::cout << "|------ Ship name: " << ship.first << std::endl;
                }
                std::cout << '|' << std::endl;
            }
            std::cout << "=========================\n" << std::endl;

            // TODO: figure out where to get the remote ShipHandle from
//            KJ_REQUIRE (ships.contains (username));
//            return ships.at (username).getShipRequest().send()
//                    .then ([&] (capnp::Response <Backend::ShipHandle::GetShipResults> results) {
//                        Spaceship ship (results.getShip());
//                        KJ_ASSERT (ship.username == username);
//                        return distributeSpaceship (ship, receiver);
//                    });
        }
        auto request = sinks.at (username).sendItemRequest();
        direction.initialise (request.initItem().initDirection());
        return request.send().ignoreResult()
//            .catch_ ([this, & sinks, & username] (kj::Exception && e) {
//            log ("Connection lost to " + username);
//            log (e.getDescription());
//            sinks.erase (username);
//        })
        ;
    }
}

/* Copyright © 2022 Aaron Alef */