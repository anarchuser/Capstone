#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed, ClientID id)
            : rng_seed {seed}
            , ID {id}
            {}

    BackendImpl::Client::Client (Registrar_t && registrar, kj::Maybe <Synchro_t> && synchro, Type type)
            : registrar {registrar}
            , synchro {synchro}
            , type {type}
            {}

    BackendImpl::Client::Client (Registrar_t && registrar)
            : Client (std::forward <Registrar_t> (registrar), {}, LOCAL)
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
        ClientID const & remoteID = params.getId();
        KJ_REQUIRE (!clients.contains (remoteID), remoteID, "Client is already registered!");
        log ("Connect request received from client " + ClientID (params.getId()));

        KJ_REQUIRE (params.hasRegistrar());
        clients.emplace (params.getId(), params.getRegistrar());

        auto results = context.getResults();
        results.setId (ID);
        auto registrar = kj::heap <RegistrarImpl>(remoteID);
        registrar->setOnRegisterShip ([this] (Spaceship const & ship, ClientID const & id, ShipHandle_t handle) {
            return registerShip (ship, id, handle);
        });
        results.setRegistrar (kj::mv (registrar));

        auto synchro = kj::heap <SynchroImpl>(remoteID);
        synchro->setOnConnect ([this] (ClientID const & id, Synchro_t synchro, Registrar_t registrar) {
            return connectCallback (id, synchro, registrar);
        });
        synchro->setOnShare ([this] (ClientID const & id, Synchro_t synchro) {
            return connectTo (id, synchro);
        });
        results.setSynchro (kj::mv (synchro));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        ClientID const & remoteID = params.getId();
        log ("Join request received from " + remoteID);

        // Share our own synchro callback to new connection
        KJ_REQUIRE (params.hasRemote());
        shareConnections (remoteID, params.getRemote());

        /// Connect back to received synchro
        return connectTo (remoteID, params.getRemote());
    }

    ::kj::Own <RegistrarImpl> BackendImpl::connectCallback (ClientID const & id, Synchro_t synchro, Registrar_t remoteRegistrar) {
        log ("Received Synchro::connect request from " + id);
        clients.emplace (id, Client (std::move (remoteRegistrar), synchro));
        shareConnections (id, synchro);

        auto registrar = kj::heap <RegistrarImpl>(id);
        registrar->setOnRegisterShip ([this] (Spaceship const & ship, ClientID const & id, ShipHandle_t handle) {
            return registerShip (ship, id, handle);
        });
        return registrar;
    }

    ::kj::Promise <void> BackendImpl::connectTo (ClientID const & id, Synchro_t synchro) {
        auto connectRequest = synchro.connectRequest();
        connectRequest.setId (ID);

        auto localSynchro = kj::heap <SynchroImpl> (id);
        localSynchro->setOnConnect ([this] (ClientID const & id, Synchro_t synchro, Registrar_t registrar) {
            return connectCallback (id, synchro, registrar);
        });
        localSynchro->setOnShare ([this] (ClientID const & id, Synchro_t synchro) {
            return connectTo (id, synchro);
        });
        connectRequest.setSynchro (kj::mv (localSynchro));

        auto registrar = kj::heap <RegistrarImpl> (id);
        registrar->setOnRegisterShip ([this] (Spaceship ship, ClientID const & id, ShipHandle_t handle) {
            return registerShip (ship, id, handle);
        });
        connectRequest.setRegistrar (kj::mv (registrar));

        return connectRequest.send().then ([this, synchro = synchro, id = id] (capnp::Response <Backend::Synchro::ConnectResults> results) mutable {
            KJ_REQUIRE (results.hasRegistrar());
            clients.emplace (id, Client (results.getRegistrar (), synchro));
            log ("Number of clients connected: "s += std::to_string (clients.size()));
        });
    }

    void BackendImpl::shareConnections (ClientID const & id, Synchro_t synchro) {
        if (clients.contains (id)) return;
        log ("Share " + id + " our own synchro");

        auto shareRequest = synchro.shareRequest();
        shareRequest.setId (ID);
        auto local = kj::heap <SynchroImpl> (id);
        local->setOnConnect ([this] (ClientID const & id, Synchro_t synchro, Registrar_t registrar) {
            return connectCallback (id, synchro, registrar);
        });
        local->setOnShare ([this] (ClientID const & id, Synchro_t synchro) {
            return connectTo (id, synchro);
        });
        shareRequest.setSynchro (kj::mv (local));
        detach (shareRequest.send().ignoreResult());

        // Share all other synchro callbacks
        for (auto & client : clients) {
            if (client.second.type == Client::REMOTE && client.first != id) {
                auto * clientSynchro = kj::_::readMaybe (client.second.synchro);
                KJ_ASSERT_NONNULL (clientSynchro);
                auto shareRequest = synchro.shareRequest ();
                shareRequest.setId (client.first);
                shareRequest.setSynchro (* clientSynchro);
                detach (shareRequest.send().ignoreResult());
            }
        }
    }

    ::kj::Own <ShipSinkImpl> BackendImpl::registerShip (Spaceship const & ship, ClientID const & id, ShipHandle_t handle) {
        KJ_REQUIRE (clients.contains (id), id, "Ship owner is not registered");
        auto & ships = clients.at (id).ships;
        auto username = ship.username;
        KJ_REQUIRE (!ships.contains (username), id, username, "Client 'id' already contains a ship named 'username'");

        log ("Registering ship " + username + " from client " + id);
        log ("Position: / "  + std::to_string (ship.position[0]) + " | " + std::to_string (ship.position[1]) + " \\");
        log ("Velocity: \\ " + std::to_string (ship.velocity[0]) + " | " + std::to_string (ship.velocity[1]) + " /");
        log ("Health: " + std::to_string (ship.health));

        auto [iterator, success] = ships.emplace (username, handle);
        KJ_ASSERT (success);

        broadcastSpaceship (ship, id);

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

    void BackendImpl::broadcastSpaceship (Spaceship const & ship, ClientID const & id) {
        log ("Number of clients: " + std::to_string (clients.size()));
        auto senderType = clients.at (id).type;
        for (auto & client : clients) {
            if (senderType == Client::REMOTE && client.second.type == Client::REMOTE) continue;

            log ("Broadcast ship " + ship.username + " to client " + client.first);
            detach (distributeSpaceship (ship, client.second));
        }
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
    kj::Promise <void> BackendImpl::doneCallback (ShipName const & username) {
        log ("Ship " + username + " is done");
        for (auto & client : clients) {

            auto & sinks = client.second.sinks;
            auto promise = sinks.contains (username)
                    ? sinks.at (username).doneRequest().send().ignoreResult()
                    : kj::READY_NOW;
            promise.then ([this, id = client.first, username] () {
                auto & client = clients.at (id);
                client.sinks.erase (username);
                client.ships.erase (username);
                if (client.ships.empty()) disconnect (id);
            }).detach ([] (...) {});  // Ignore errors on done
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::sendItemCallback (ShipName const & username, Direction const & direction, ClientID const & id) {
        KJ_REQUIRE (clients.contains (id), id, username, "Received item from unregistered client");
        auto & sender = clients.at (id);
        if (sender.ships.empty()) {
            disconnect (id);
            return kj::READY_NOW;
        }
        KJ_ASSERT (sender.ships.contains (username), id, username, "Client does not own ship 'username'");

        /// Case 1: Item got sent from a local client -> Distribute to everyone
        if (sender.type == Client::LOCAL) {
//            log ("Send Item callback from local game client");
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
//            log ("Send Item callback from remote synchro client");
            for (auto & client : clients) {
                // Do not send the item to remote clients, as they've received the item directly already
                if (client.second.type == Client::REMOTE) continue;

                // Send the item to the corresponding spaceship of our own local clients, or create it if needed
                sendItemToClient (username, direction, client.second).detach (
                        [this, id = client.first] (kj::Exception && e) { disconnect (id); });
            }
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::sendItemToClient (ShipName const & username, Direction const & direction, Client & receiver) {
        auto & sinks = receiver.sinks;

        if (!sinks.contains (username)) {
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
                .catch_ ([this, & sinks, & username] (kj::Exception && e) { sinks.erase (username); });
    }

    void BackendImpl::disconnect (ClientID const & id) {
        if (!clients.contains (id)) return;
        log ("Disconnecting client " + id);

        for (auto & sink : clients.at (id).sinks) {
            sink.second.doneRequest().send().detach ([](...){});  // Ignore errors on done
        }
        clients.erase (id);
    }
}

/* Copyright © 2022 Aaron Alef */