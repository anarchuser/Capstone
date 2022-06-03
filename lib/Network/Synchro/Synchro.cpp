#include "Synchro.h"

namespace cg {
    SynchroImpl::SynchroImpl (ClientID id, std::optional<LocalClient> & local,
                              std::unordered_map<ClientID, RemoteClient> & remotes)
            : ID {std::move (id)}
            , local {local}
            , remotes {remotes}
            {}

    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro {" << ID << "} @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        debug_stdout (ss.str());
    }

    ::kj::Promise <void> SynchroImpl::connect (ConnectContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        log ("Connection request received from client " + std::string (params.getId()));
        std::string id = params.getId();
        KJ_REQUIRE (id != ID, "Remote client cannot have our own identifier");

        KJ_REQUIRE (params.hasRegistrar());
        auto registrar = params.getRegistrar();

        KJ_REQUIRE (params.hasSynchro());
        auto remote = params.getSynchro();

        remotes.emplace (id, RemoteClient (registrar, remote));
        shareConnections (id, remote);

        auto results = context.getResults();
        results.setRegistrar (newRegistrar (id));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> SynchroImpl::share (ShareContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        KJ_REQUIRE (params.hasSynchro());

        log ("Client " + std::string (params.getId()) + " shared us a connection");
        return connectTo (params.getId(), params.getSynchro());
    }

    ::kj::Promise <void> SynchroImpl::connectTo (ClientID const & id, Synchro_t remote) {
        KJ_DASSERT (id != ID, id, "Cannot connect to client with our own identifier");
        log ("Connecting to client " + id);

        auto connectRequest = remote.connectRequest();
        connectRequest.setId (ID);
        connectRequest.setRegistrar (newRegistrar (id));
        connectRequest.setSynchro   (newSynchro   (id));

        return connectRequest.send().then ([this, remote, id] (auto results) mutable {
            KJ_REQUIRE (results.hasRegistrar());
            log ("Connection request to " + id + " returned");
            remotes.emplace (id, RemoteClient (results.getRegistrar (), remote));
            log ("Number of remote clients connected: "s += std::to_string (remotes.size()));
        });
    }

    void SynchroImpl::shareConnections (ClientID const & id, Synchro_t remote) {
        if (remotes.contains (id)) return;
        log ("Share our own remote to client " + id);

        auto shareRequest = remote.shareRequest();
        shareRequest.setId (ID);
        shareRequest.setSynchro (newSynchro (id));
        shareRequest.send().detach ([this, id] (kj::Exception && e) {
            KJ_LOG (WARNING, "Failed to send a request to remote client " + id, e.getDescription());
        });

        // Share all other remote callbacks
        for (auto & client : remotes) {
            if (client.first == id) continue;

            log ("Share remote client " + client.first + " to client " + id);
            auto shareRequest = remote.shareRequest ();
            shareRequest.setId (client.first);
            shareRequest.setSynchro (client.second.synchro);
            shareRequest.send().ignoreResult().detach ([this, id] (kj::Exception && e) {
                KJ_LOG (WARNING, "Failed to send a request to remote client " + id, e.getDescription());
            });
        }
    }

    ::kj::Own <RegistrarImpl> SynchroImpl::newRegistrar (ClientID const & id) {
        auto registrar = kj::heap <RegistrarImpl> (id);
        registrar->setOnRegisterShip (LAMBDA (registerShip));
        return registrar;
    }
    ::kj::Own <SynchroImpl> SynchroImpl::newSynchro (ClientID id) {
        return kj::heap <SynchroImpl> (id, local, remotes);
    }

    ::kj::Own <ShipSinkImpl> SynchroImpl::registerShip (Spaceship const & ship, ClientID const & id, ShipHandle_t handle) {
        auto * client = findClient (id);
        KJ_REQUIRE_NONNULL (client, id, "Ship owner is not registered");
        auto & ships = client->ships;
        auto username = ship.username;
        KJ_REQUIRE (!ships.contains (username), id, username, "Client 'id' already contains a ship named 'username'");

        log ("Registering ship " + username + " from client " + id);
        log ("Position: / "  + std::to_string (ship.position.x) + " | " + std::to_string (ship.position.y) + " \\");
        log ("Velocity: \\ " + std::to_string (ship.velocity.x) + " | " + std::to_string (ship.velocity.y) + " /");
        log ("Health: " + std::to_string (ship.health));

        ships.emplace (username, handle);
        broadcastSpaceship (ship, handle, remotes.contains (id));

        // Prepare ShipSink
        auto sink = kj::heap <ShipSinkImpl>();
        sink->setOnDone ([this, username] () {
            return doneCallback (username);
        });
        sink->setOnSendItem ([this, id] (Item const & item) {
            return sendItemCallback (item, id);
        });
        return sink;
    }

    void SynchroImpl::broadcastSpaceship (Spaceship const & ship, ShipHandle_t handle, bool isRemote) {
        if (local.has_value()) {
            log ("Broadcast ship " + ship.username + " to local client");
            detach (distributeSpaceship (ship, handle, local.value()));
        }
        for (auto & client : remotes) {
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
        return request.send().then ([this, username, & receiver] (auto results) {
            // Check again if nothing's changed
            if (receiver.sinks.contains (username)) return;
            receiver.sinks.insert_or_assign (username, results.getSink ());
        });
    }
    void SynchroImpl::doneCallback (ShipName const & username) {
        log ("Ship " + username + " is done");
        for (auto & client : remotes) {
            detach (client.second.erase (username)
                            .then ([this, id = client.first] () {disconnect (id);}));
        }
    }
    void SynchroImpl::sendItemCallback (Item const & item, ClientID const & id) {
        auto * sender = findClient (id);
        auto const & [direction, data] = item;
        auto const & username = data.username;
        KJ_REQUIRE_NONNULL (sender, id, username, "Received item from unregistered client");
        if (sender->ships.empty()) {
            disconnect (id);
            return;
        }
                KJ_ASSERT (sender->ships.contains (username), id, username, "Client does not own ship 'username'");
        auto & handle = sender->ships.at (username);

        // If item came from local client -> distribute it to all remote clients
        if (id == ID) {
            for (auto & client : remotes) {
                sendItemToClient (item, handle, client.second).detach (
                        [this, id = client.first] (kj::Exception && e) { disconnect (id); });
            }
        }
        // Distribute item to all local clients
        if (local.has_value()) {
            detach (sendItemToClient (item, handle, local.value()));
        }
    }
    kj::Promise <void> SynchroImpl::sendItemToClient (Item const & item, ShipHandle_t handle, Client & receiver) {
        auto & sinks = receiver.sinks;
        auto const & [direction, data] = item;
        auto const & username = data.username;

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
        remotes.erase (id);
    }

    Client * SynchroImpl::findClient (ClientID const & id) {
        if (remotes.contains (id)) return & remotes.at (id);
        if (id == ID) {
            if (local.has_value()) return & local.value();

            KJ_LOG (WARNING, "Local client requested but it does not exist (yet)");
        }
        return nullptr;
    }
}

/* Copyright © 2022 Aaron Alef */
