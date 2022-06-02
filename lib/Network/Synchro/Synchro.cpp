#include "Synchro.h"

namespace cg {
    SynchroImpl::SynchroImpl (ClientID const & id, std::optional<LocalClient> & local,
                              std::unordered_map<ClientID, RemoteClient> & remote)
            : ID {id}
            , local {local}
            , remote {remote}
            {}

    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro " << ID << " @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
#ifdef DEBUG_MINOR
        debug_stdout (ss.str());
#endif
    }

    ::kj::Promise<void> SynchroImpl::connect (ConnectContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        log ("Connection request received from client " + std::string (params.getId()));

        KJ_REQUIRE (params.hasSynchro());
        KJ_REQUIRE (params.hasRegistrar());
        auto results = context.getResults();

        try {
            results.setRegistrar (onConnect (params.getId(), params.getSynchro(), params.getRegistrar()));
            results.setId (ID);
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "Synchro::connect called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> SynchroImpl::share (ShareContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        KJ_REQUIRE (params.hasSynchro());

        log ("Shared connection received from " + std::string (params.getId()));

        try {
            return onShare (params.getId(), params.getSynchro());
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "Synchro::share called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Own <RegistrarImpl> SynchroImpl::connectCallback (ClientID const & id, Synchro_t synchro, Registrar_t remoteRegistrar) {
        log ("Received Synchro::connect request from " + id);
        KJ_REQUIRE (id != ID, id, "Remote client cannot have our own identifier");

        remote.emplace (id, RemoteClient (remoteRegistrar, synchro));
        shareConnections (id, synchro);

        return newRegistrar (id);
    }

    ::kj::Promise <void> SynchroImpl::connectTo (ClientID const & id, Synchro_t synchro) {
                KJ_DASSERT (id != ID, id, "Cannot connect to client with our own identifier");

        auto connectRequest = synchro.connectRequest();
        connectRequest.setId (ID);
        connectRequest.setRegistrar (newRegistrar (id));
        connectRequest.setSynchro   (newSynchro   (id));

        return connectRequest.send().then ([this, synchro, id] (auto results) mutable {
            KJ_REQUIRE (results.hasRegistrar());
            remote.emplace (id, RemoteClient (results.getRegistrar (), synchro));
            log ("Number of remote clients connected: "s += std::to_string (remote.size()));
        });
    }

    void SynchroImpl::shareConnections (ClientID const & id, Synchro_t synchro) {
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

    ::kj::Own <RegistrarImpl> SynchroImpl::newRegistrar (ClientID const & id) {
        auto registrar = kj::heap <RegistrarImpl> (id);
        registrar->setOnRegisterShip (LAMBDA (registerShip));
        return registrar;
    }
    ::kj::Own <SynchroImpl> SynchroImpl::newSynchro (ClientID const & id) {
        auto localSynchro = kj::heap <SynchroImpl> (* this);
        localSynchro->setOnConnect (LAMBDA (connectCallback));
        localSynchro->setOnShare (LAMBDA (connectTo));
        return localSynchro;
    }

    ::kj::Own <ShipSinkImpl> SynchroImpl::registerShip (Spaceship const & ship, ClientID const & id, ShipHandle_t handle) {
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

    void SynchroImpl::broadcastSpaceship (Spaceship const & ship, ShipHandle_t handle, bool isRemote) {
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
    kj::Promise <void> SynchroImpl::distributeSpaceship (Spaceship const & ship, ShipHandle_t handle, Client & receiver) {
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
    void SynchroImpl::doneCallback (ShipName const & username) {
        log ("Ship " + username + " is done");
        for (auto & client : remote) {
            detach (client.second.erase (username)
                            .then ([this, id = client.first] () {disconnect (id);}));
        }
    }
    void SynchroImpl::sendItemCallback (Direction const & direction, Spaceship const & data, ClientID const & id) {
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
    kj::Promise <void> SynchroImpl::sendItemToClient (Direction const & direction, Spaceship const & data, ShipHandle_t handle, Client & receiver) {
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

    kj::Promise <void> SynchroImpl::disconnect (ClientID const & id) {
        log ("Disconnecting client " + id);
        auto * client = findClient (id);
        if (!client) return kj::READY_NOW;

        // TODO: tie each promise in the loop to the returned promise
        for (auto const & sink : client->sinks) {
            detach (client->erase (sink.first));
        }
        remote.erase (id);
    }

    Client * SynchroImpl::findClient (ClientID const & id) {
        if (remote.contains (id)) return & remote.at (id);
        if (id == ID) {
            if (local.has_value()) return & local.value();

            KJ_LOG (WARNING, "Local client requested but it does not exist (yet)");
        }
        return nullptr;
    }
}

/* Copyright © 2022 Aaron Alef */
