#include "Backend.h"

namespace cg {
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
        KJ_REQUIRE (!local.contains (remoteID), remoteID, "Client is already registered locally!");
        KJ_REQUIRE (!remote.contains (remoteID), remoteID, "Client is already registered remotely!");
        log ("Connect request received from local client " + ClientID (params.getId()));

        KJ_REQUIRE (params.hasRegistrar());
        local.emplace (params.getId(), params.getRegistrar());

        auto results = context.getResults();
        results.setId (ID);
        results.setRegistrar (newRegistrar (remoteID));
        results.setSynchro   (newSynchro   (remoteID));

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
        remote.emplace (id, RemoteClient (remoteRegistrar, synchro));
        shareConnections (id, synchro);

        return newRegistrar (id);
    }

    ::kj::Promise <void> BackendImpl::connectTo (ClientID const & id, Synchro_t synchro) {
        auto connectRequest = synchro.connectRequest();
        connectRequest.setId (ID);
        connectRequest.setRegistrar (newRegistrar (id));
        connectRequest.setSynchro   (newSynchro   (id));

        return connectRequest.send().then ([this, synchro = synchro, id = id] (capnp::Response <Backend::Synchro::ConnectResults> results) mutable {
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
        shareRequest.setSynchro (newSynchro (id));
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
    ::kj::Own <SynchroImpl> BackendImpl::newSynchro (ClientID const & id) {
        auto localSynchro = kj::heap <SynchroImpl> (id);
        localSynchro->setOnConnect (LAMBDA (connectCallback));
        localSynchro->setOnShare (LAMBDA (connectTo));
        return localSynchro;
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
        sink->setOnSendItem ([this, username, id] (Direction const & direction) {
            return sendItemCallback (username, direction, id);
        });
        return sink;
    }

    void BackendImpl::broadcastSpaceship (Spaceship const & ship, ShipHandle_t handle, bool isRemote) {
        for (auto & client : remote) {
            if (isRemote) continue;

            log ("Broadcast ship " + ship.username + " to remote client " + client.first);
            detach (distributeSpaceship (ship, handle, client.second));
        }
        for (auto & client : local) {
            log ("Broadcast ship " + ship.username + " to local client " + client.first);
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
    kj::Promise <void> BackendImpl::doneCallback (ShipName const & username) {
        log ("Ship " + username + " is done");
        // TODO: tie each promise in the loop to the returned promise
        for (auto & client : remote) {
            detach (client.second.erase (username).then ([this, id = client.first] () {
                if (remote.contains (id)) disconnect (id);
                if (local.contains (id)) disconnect (id);
            }));
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::sendItemCallback (ShipName const & username, Direction const & direction, ClientID const & id) {
        auto * sender = findClient (id);
        KJ_REQUIRE_NONNULL (sender, id, username, "Received item from unregistered client");
        if (sender->ships.empty()) {
            disconnect (id);
            return kj::READY_NOW;
        }
        KJ_ASSERT (sender->ships.contains (username), id, username, "Client does not own ship 'username'");
        auto & handle = sender->ships.at (username);

        // If item came from local client -> distribute it to all remote clients
        if (local.contains (id)) {
            // TODO: tie each promise in the loop to the returned promise
            for (auto & client : remote) {
                sendItemToClient (username, direction, handle, client.second).detach (
                        [this, id = client.first] (kj::Exception && e) { disconnect (id); });
            }
        }
        // TODO: tie each promise in the loop to the returned promise
        // Distribute item to all local clients
        for (auto & client : local) {
            sendItemToClient (username, direction, handle, client.second).detach (
                    [this, id = client.first] (kj::Exception && e) { disconnect (id); });
        }
        return kj::READY_NOW;
    }
    kj::Promise <void> BackendImpl::sendItemToClient (ShipName const & username, Direction const & direction, ShipHandle_t handle, Client & receiver) {
        auto & sinks = receiver.sinks;

        if (!sinks.contains (username)) {
            return handle.getShipRequest().send()
                    .then ([this, handle, & receiver] (auto results) mutable {
                        return distributeSpaceship (Spaceship (results.getShip()), handle, receiver);
                    });
        }
        auto request = sinks.at (username).sendItemRequest();
        direction.initialise (request.initItem().initDirection());
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
        if (local.contains  (id)) return & local.at  (id);
        return nullptr;
    }
}

/* Copyright © 2022 Aaron Alef */