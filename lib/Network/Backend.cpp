#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed, std::string name)
            : rng_seed {seed}
            , name {name} {}

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

    kj::Own <ShipHandleImpl> BackendImpl::registerShipCallback (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
        std::string const & username = spaceship.username;
        KJ_REQUIRE (connections.contains (username), username, "Tried to register a ship without being registered");

        auto & shipHandles = connections.at (username).shipHandles;

        // If username exists already, replace:
        if (shipHandles.contains (username)) {
            doneCallback (username);
        }
        KJ_REQUIRE (! shipHandles.contains (username), username, "Username already in use");

        log (std::string ("Join request received from user ") + username);
        log (std::string ("Position: ( ")
             + std::to_string (spaceship.position[0]) + " | "
             + std::to_string (spaceship.position[1]) + " )");
        log (std::string ("Velocity: ( ")
                + std::to_string (spaceship.velocity[0]) + " | "
                + std::to_string (spaceship.velocity[1]) + " )");
        log (std::string ("Health: " + std::to_string (spaceship.health)));

        // Store connection details (callback handles and 'new ship' callback)
        log ("Store connection details");
        auto [iterator, success] = shipHandles.emplace (username, handle);
        KJ_ASSERT (success);

        broadcastSpaceship (spaceship);

        log ("Send ShipHandle back to " + username);
        auto sink = kj::heap <ShipHandleImpl> ();
        sink->setOnSendItem ([this, username] (Direction direction) {
            sendItemCallback (username, direction);
        });
        sink->setOnDone ([this, username] () {
            doneCallback (username);
        });

        return kj::mv (sink);
    }

    kj::Own <ShipRegistrarImpl> BackendImpl::exchangeRegistrars (std::string const & name, Backend::ShipRegistrar::Client remote) {
        log ("Exchanging registrars");

        connections.emplace (name, Connection {remote});
        KJ_ASSERT (connections.contains (name));

        auto local = kj::heap <ShipRegistrarImpl> ();
        local->setOnRegisterShip ([this] (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
            return registerShipCallback (spaceship, handle);
        });
        return local;
    }

    void BackendImpl::sendItemCallback (std::string const & sender, Direction direction) {
        for (auto & connection : connections) {
            auto & shipHandles = connection.second.shipHandles;

            if (!shipHandles.contains (sender)) {
                connections.at (sender).shipHandles.at (sender).getSpaceshipRequest ().send().then (
                        [&] (capnp::Response <Backend::ShipHandle::GetSpaceshipResults> response) {
                            distributeSpaceship (Spaceship (response.getSpaceship()), connection.first);
                            sendItemCallback (sender, direction);
                        }).detach ([&] (kj::Exception && e) {
                            KJ_DLOG (WARNING, "Exception on establishing missing connection: ", e.getDescription ());
                            connections.erase (connection.first);
                        });
                return;
            }
            KJ_REQUIRE (shipHandles.contains (sender), sender, "Cannot forward directions; ship handle not found");

            auto request = shipHandles.at (sender).sendItemRequest();
            direction.initialise (request.initItem().initDirection());
            request.send().then ([](...){});
        }
    }

    void BackendImpl::doneCallback (std::string const & sender) {
        for (auto & connection : connections) {
            auto & shipHandles = connection.second.shipHandles;
            if (shipHandles.contains (sender)) {
                log ("Closing sinks from " + sender);

                auto handle = std::move (shipHandles.at (sender));
                shipHandles.erase (sender);

                // TODO: execute handle.doneRequest() here
            }
        }
    }

    void BackendImpl::distributeSpaceship (Spaceship const & sender, std::string const & receiver) {
        log ("Distributing spaceship from " + sender.username + " to " + receiver);

        KJ_REQUIRE (connections.contains (receiver));
        KJ_REQUIRE (connections.contains (sender.username));

        auto & connection = connections.at (receiver);
        auto & shipHandles = connection.shipHandles;
        if (shipHandles.contains (sender.username)) {
            log ("Connection exists already");
            return;
        };

        auto & registrar = connection.registrar;
        auto request = registrar.registerShipRequest();
        sender.initialise (request.initSpaceship());

        request.send().then ([&] (capnp::Response <Backend::ShipRegistrar::RegisterShipResults> results) {
            KJ_REQUIRE (results.hasRemote());
            auto [iterator, success] = shipHandles.emplace (sender.username, results.getRemote());
            KJ_ASSERT (success);
        }).detach ([&] (kj::Exception && e) {
            KJ_DLOG (WARNING, "Exception on registering ship to client", e.getDescription ());
            connections.erase (receiver);
        });
    }

    void BackendImpl::broadcastSpaceship (Spaceship const & sender) {
        for (auto & connection : connections) {
            distributeSpaceship (sender, connection.first);
        }
    }

    ::kj::Promise <void> BackendImpl::registerClient (RegisterClientContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasS2c_registrar());
        std::string name = params.getName();

        log ("Registering game client: " + name);

        connections.insert_or_assign (name, Connection {params.getS2c_registrar()});

        auto results = context.initResults();
        results.setC2s_registrar (exchangeRegistrars (params.getName(), params.getS2c_registrar()));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::synchro (SynchroContext context) {
        log ("Synchro requested");

        auto synchro = kj::heap <cg::SynchroImpl> ();
        synchro->setOnConnect ([this] (std::string const & name, Backend::ShipRegistrar::Client client) {
            return exchangeRegistrars (name, client);
        });
        context.initResults().setRemote (kj ::mv (synchro));
        context.getResults().setName (name);

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */