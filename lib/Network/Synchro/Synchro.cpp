#include "Synchro.h"

namespace cg {
    SynchroImpl::SynchroImpl (ClientID id, std::optional<LocalClient> & local,
                              std::unordered_map<ClientID, RemoteClient> & remotes)
            : ID {std::move (id)}
            , local {local}
            , remotes {remotes}
            {}

    void SynchroImpl::log (std::string const & msg) const {
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

        auto results = context.getResults();
        results.setRegistrar (newRegistrar (id));

        return shareConnections (id, remote);
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

    kj::Promise <void> SynchroImpl::shareConnections (ClientID const & id, Synchro_t remote) {
        if (remotes.contains (id)) return kj::READY_NOW;
        log ("Share our own remote to client " + id);

        kj::Vector <kj::Promise <void>> promises;

        auto shareRequest = remote.shareRequest();
        shareRequest.setId (ID);
        shareRequest.setSynchro (newSynchro (ID));
        promises.add (shareRequest.send().ignoreResult());

        // Share all other remote callbacks
        for (auto & client : remotes) {
            if (client.first == id) continue;

            log ("Share remote client " + client.first + " to client " + id);
            auto shareRequest = remote.shareRequest ();
            shareRequest.setId (client.first);
            shareRequest.setSynchro (client.second.synchro);
            promises.add (shareRequest.send().ignoreResult());
        }
        return kj::joinPromises (promises.releaseAsArray());
    }

    ::kj::Own <RegistrarImpl> SynchroImpl::newRegistrar (ClientID const & id) {
        auto registrar = kj::heap <RegistrarImpl> ();
        registrar->setOnRegisterShip ([this, id] (Spaceship const & ship, ShipHandle_t handle) {
            return registerShip (ship, id, handle);
        });
        return registrar;
    }
    ::kj::Own <SynchroImpl> SynchroImpl::newSynchro (ClientID const & id) {
        return kj::heap <SynchroImpl> (id, local, remotes);
    }

    kj::Promise <void> SynchroImpl::disconnect (ClientID const & id) {
        auto * client = findClient (id);
        if (!client) return kj::READY_NOW;

        log ("Disconnecting " + id);
        return client->destroy().then ([this, id] () {
            removeClient (id);
        });
    }

    Client * SynchroImpl::findClient (ClientID const & id) {
        if (remotes.contains (id)) return & remotes.at (id);
        if (id == ID) {
            if (local.has_value()) return & local.value();
            KJ_LOG (WARNING, "Local client requested but it does not exist (yet)");
        }
        return nullptr;
    }
    kj::Promise <void> SynchroImpl::removeClient (ClientID const & id) {
        if (id == ID) {
            local.reset();
            return kj::READY_NOW;
        }
        remotes.erase (id);
        return kj::READY_NOW;
    }

    kj::Own <ShipSinkImpl> SynchroImpl::registerShip (Spaceship const & ship, ClientID const & id, ShipHandle_t handle) {
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

        if (local.has_value()) {
            log ("Register ship to local client");
            detach (local->registerShip (ship, handle));
        }
        if (id == ID) {
            log ("Register ship to remote clients");
            // Item was sent from owned ship -> distribute to all remote clients
            detach (broadcastSpaceship (ship, handle));
        }

        // Prepare ShipSink
        auto sink = kj::heap <ShipSinkImpl>();
        sink->setOnDone ([this, username] () {
            return doneCallback (username);
        });
        sink->setOnSendItem ([this, id] (Item const & item) {
            return sendItemCallback (item, id);
        });
        sink->setOnGetShip ([this, username] () -> kj::Promise <Spaceship> {
            KJ_REQUIRE (local.has_value());
            KJ_REQUIRE (local->sinks.contains (username));
            return local->sinks.at (username).getShipRequest().send().then ([] (auto results) {
                return Spaceship (results.getShip());
            });
        });
        return kj::mv (sink);
    }

    kj::Promise <void> SynchroImpl::broadcastSpaceship (Spaceship const & ship, ShipHandle_t handle) {
        kj::Vector <kj::Promise <void>> promises;
        for (auto & client : remotes) {
            log ("Broadcast ship " + ship.username + " to remote client " + client.first);
            promises.add (client.second.registerShip (ship, handle));
        }
        return kj::joinPromises (promises.releaseAsArray());
    }

    kj::Promise <void> SynchroImpl::doneCallback (ShipName const & username) {
        log ("Ship " + username + " is done");
        kj::Vector <kj::Promise <void>> promises;
        if (local.has_value()) {
            local->ships.erase (username);
            if (local->ships.empty()) {
                log ("Local client has no ships anymore - disconnecting");

                promises.add (local->destroy());
                local.reset();

                for (auto & client : remotes) {
                    promises.add (client.second.destroy());
                }
                remotes.clear();
                return kj::joinPromises (promises.releaseAsArray());
            }
            promises.add (local->erase (username));
        }
        for (auto & client : remotes) {
            promises.add (client.second.erase (username));
        }
        return kj::joinPromises (promises.releaseAsArray());
    }
    kj::Promise <void> SynchroImpl::sendItemCallback (Item const & item, ClientID const & id) {
        auto * sender = findClient (id);
        auto const & [time, direction, data] = item;
        auto const & username = data.username;
        KJ_REQUIRE_NONNULL (sender, id, username, "Received item from unregistered client");
        if (sender->ships.empty()) return disconnect (id);

        KJ_ASSERT (sender->ships.contains (username), id, username, "Client does not own ship 'username'");
        auto & handle = sender->ships.at (username);

        if (id == ID) {
            // Item came from local client -> distribute to all remote clients
            for (auto & client : remotes) {
                client.second.sendItem (item, handle)
                        .detach ([this, id = client.first] (kj::Exception && e) {
                            KJ_DLOG (WARNING, "Sending item to remote client " + id + " failed", e.getDescription());
                            disconnect (id);
                        });
            }
        }
        if (local.has_value()) {
            // Distribute to local client
            return local->sendItem (item, handle);
        }
        /* All cases should have been handled */
        KJ_UNREACHABLE
    }
}

/* Copyright © 2022 Aaron Alef */
