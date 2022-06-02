#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed, ClientID id)
            : rng_seed {seed}
            , ID {std::move (id)}
            , local {}
            , remote {}
            , synchro (ID, local, remote)
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
        log ("Connect request received from local client " + ClientID (params.getId()));
        KJ_REQUIRE (remoteID == ID, remoteID, "Local client ID must have our backend ID");
        KJ_REQUIRE (!local, "A local client is registered already");

        KJ_REQUIRE (params.hasRegistrar());
        local.emplace (params.getRegistrar());

        auto results = context.getResults();
        results.setId (ID);
        results.setRegistrar (newRegistrar (remoteID));
        results.setSynchro (synchro.newSynchro (remoteID));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        ClientID const & remoteID = params.getId();
        log ("Join request received from " + remoteID);
        KJ_REQUIRE (remoteID != ID, remoteID, "Remote client cannot have our own identifier");

        // Share our own synchro callback to new connection
        KJ_REQUIRE (params.hasRemote());
        shareConnections (remoteID, params.getRemote());

        /// Connect back to received synchro
        return connectTo (remoteID, params.getRemote());
    }

    ::kj::Promise <void> BackendImpl::connectTo (ClientID const & id, Synchro_t synchro) {
        KJ_DASSERT (id != ID, id, "Cannot connect to client with our own identifier");

        auto connectRequest = synchro.connectRequest();
        connectRequest.setId (ID);
        connectRequest.setRegistrar (newRegistrar (id));
        connectRequest.setSynchro (this->synchro.newSynchro (id));

        return connectRequest.send().then ([this, synchro, id] (auto results) mutable {
            KJ_REQUIRE (results.hasRegistrar());
            remote.emplace (id, RemoteClient (results.getRegistrar (), synchro));
            log ("Number of remote clients connected: "s += std::to_string (remote.size()));
        });
    }

    void BackendImpl::shareConnections (ClientID const & id, Synchro_t synchro) {
        if (remote.contains (id)) return;
        log ("Share " + id + " our own synchro");

        auto shareRequest = synchro.shareRequest();
        shareRequest.setId (ID);
        shareRequest.setSynchro (this->synchro.newSynchro (ID));
        detach (shareRequest.send().ignoreResult());

        // Share all other synchro callbacks
        for (auto & client : remote) {
            if (client.first != id) {
                auto shareRequest = synchro.shareRequest ();
                shareRequest.setId (client.first);
                shareRequest.setSynchro (client.second.synchro);
                detach (shareRequest.send().ignoreResult());
            }
        }
    }

    ::kj::Own <RegistrarImpl> BackendImpl::newRegistrar (ClientID const & id) {
        auto registrar = kj::heap <RegistrarImpl> (id);
        registrar->setOnRegisterShip (LAMBDA (registerShip));
        return registrar;
    }

    ::kj::Own <ShipSinkImpl> BackendImpl::registerShip (Spaceship const & ship, ClientID const & id, ShipHandle_t handle) {
        auto * client = findClient (id);
        KJ_REQUIRE_NONNULL (client, id, "Ship owner is not registered");
        auto & ships = client->ships;
        auto username = ship.username;
        KJ_REQUIRE (!ships.contains (username), id, username, "Client 'id' already contains a ship named 'username'");

        log ("Registering ship " + username + " from client " + id);
        log ("Position: / "  + std::to_string (ship.position[0]) + " | " + std::to_string (ship.position[1]) + " \\");
        log ("Velocity: \\ " + std::to_string (ship.velocity[0]) + " | " + std::to_string (ship.velocity[1]) + " /");
        log ("Health: " + std::to_string (ship.health));

        ships.emplace (username, handle);
        broadcastSpaceship (ship, handle, remote.contains (id));

        // Prepare ShipSink
        auto sink = kj::heap <ShipSinkImpl>();
        sink->setOnDone ([this, username] () {
            return doneCallback (username);
        });
        sink->setOnSendItem ([this, id] (Direction const & direction, Spaceship const & data) {
            return sendItemCallback (direction, data, id);
        });
        return sink;
    }

    void BackendImpl::broadcastSpaceship (Spaceship const & ship, ShipHandle_t handle, bool isRemote) {
        if (local.has_value()) {
            log ("Broadcast ship " + ship.username + " to local client");
            detach (distributeSpaceship (ship, handle, local.value()));
        }
        for (auto & client : remote) {
            if (isRemote) continue;

            log ("Broadcast ship " + ship.username + " to remote client " + client.first);
            detach (distributeSpaceship (ship, handle, client.second));
        }
    }
    kj::Promise <void> BackendImpl::distributeSpaceship (Spaceship const & ship, ShipHandle_t handle, Client & receiver) {
        auto & username = ship.username;
        if (receiver.sinks.contains (username)) return kj::READY_NOW;

        auto request = receiver.registrar.registerShipRequest ();
        ship.initialise (request.initShip ());
        request.setHandle (handle);
        return request.send().then ([this, username, & receiver]
                (capnp::Response<Backend::Registrar::RegisterShipResults> results) {
                    // Check again if nothing's changed
                    if (receiver.sinks.contains (username)) return;
                    receiver.sinks.insert_or_assign (username, results.getSink ());
                });
    }
    void BackendImpl::doneCallback (ShipName const & username) {
        log ("Ship " + username + " is done");
        for (auto & client : remote) {
            detach (client.second.erase (username)
                    .then ([this, id = client.first] () {disconnect (id);}));
        }
    }
    void BackendImpl::sendItemCallback (Direction const & direction, Spaceship const & data, ClientID const & id) {
        auto * sender = findClient (id);
        auto & username = data.username;
        KJ_REQUIRE_NONNULL (sender, id, username, "Received item from unregistered client");
        if (sender->ships.empty()) {
            disconnect (id);
            return;
        }
        KJ_ASSERT (sender->ships.contains (username), id, username, "Client does not own ship 'username'");
        auto & handle = sender->ships.at (username);

        // If item came from local client -> distribute it to all remote clients
        if (id == ID) {
            for (auto & client : remote) {
                sendItemToClient (direction, data, handle, client.second).detach (
                        [this, id = client.first] (kj::Exception && e) { disconnect (id); });
            }
        }
        // Distribute item to all local clients
        if (local.has_value()) {
            detach (sendItemToClient (direction, data, handle, local.value()));
        }
    }
    kj::Promise <void> BackendImpl::sendItemToClient (Direction const & direction, Spaceship const & data, ShipHandle_t handle, Client & receiver) {
        auto & sinks = receiver.sinks;
        auto & username = data.username;

        if (!sinks.contains (username)) {
            return handle.getShipRequest().send()
                    .then ([this, handle, & receiver] (auto results) mutable {
                        return distributeSpaceship (Spaceship (results.getShip()), handle, receiver);
                    });
        }
        auto request = sinks.at (username).sendItemRequest();
        direction.initialise (request.initItem().initDirection());
        data.initialise (request.getItem().initSpaceship());
        return request.send().ignoreResult()
                .catch_ ([this, & sinks, & username] (kj::Exception && e) { sinks.erase (username); });
    }

    kj::Promise <void> BackendImpl::disconnect (ClientID const & id) {
        log ("Disconnecting client " + id);
        auto * client = findClient (id);
        if (!client) return kj::READY_NOW;

        // TODO: tie each promise in the loop to the returned promise
        for (auto const & sink : client->sinks) {
            detach (client->erase (sink.first));
        }
        remote.erase (id);
    }

    Client * BackendImpl::findClient (ClientID const & id) {
        if (remote.contains (id)) return & remote.at (id);
        if (id == ID) {
            if (local.has_value()) return & local.value();

            KJ_LOG (WARNING, "Local client requested but it does not exist (yet)");
        }
        return nullptr;
    }
}

/* Copyright © 2022 Aaron Alef */