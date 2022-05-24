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

    ::kj::Promise <void> BackendImpl::connect (ConnectContext context) {
        log ("Connect request received");

        auto params = context.getParams();
        KJ_REQUIRE (params.hasName());
        KJ_REQUIRE (params.hasS2c_registrar());
        auto [iterator, success] = connections.emplace (params.getName(), params.getS2c_registrar());
        KJ_ASSERT (success);

        auto registrar = kj::heap <ShipRegistrarImpl> ();
        registrar->setOnRegisterShip ([this] (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
            return registerShipCallback (spaceship, handle);
        });

        auto results = context.getResults();
        results.setC2s_registrar(kj::mv (registrar));
        results.setRemote (name);

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        log ("Join request received");

        auto params = context.getParams();
        KJ_REQUIRE (params.hasRemote());
        auto request = params.getRemote().connectRequest ();
        request.setName (name);
        auto registrar = kj::heap <ShipRegistrarImpl> ();
        registrar->setOnRegisterShip ([this] (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
            return registerShipCallback (spaceship, handle);
        });
        request.setS2c_registrar (kj::mv (registrar));
        return request.send().then ([&] (capnp::Response <Backend::ConnectResults> results) {
            KJ_REQUIRE (results.hasC2s_registrar());
            auto [iterator, success] = connections.emplace (results.getRemote(), results.getC2s_registrar());
            KJ_ASSERT (success);
        });
    }

    kj::Own <ShipHandleImpl> BackendImpl::registerShipCallback (Spaceship const & spaceship, Backend::ShipHandle::Client handle) {
        std::string const & username = spaceship.username;
        KJ_REQUIRE (connections.contains (username), username, "Tried to register a ship without being registered");

        auto & shipHandles = connections.at (username).shipHandles;

        // If username exists already, replace:
//        if (shipHandles.contains (username)) {
//            KJ_DLOG (WARNING, "Duplicate username detected");
//            doneCallback (username);
//        }
        KJ_REQUIRE (! shipHandles.contains (username), username, "Username already in use");

        log (std::string ("Ship registration received from ") + username);
        log (std::string ("Position: ( ")
             + std::to_string (spaceship.position[0]) + " | "
             + std::to_string (spaceship.position[1]) + " )");
        log (std::string ("Velocity: ( ")
                + std::to_string (spaceship.velocity[0]) + " | "
                + std::to_string (spaceship.velocity[1]) + " )");
        log (std::string ("Health: " + std::to_string (spaceship.health)));

        // Store connection details (callback handles and 'new ship' callback)
        log ("Store connection details");
        auto [iterator, success] = shipHandles.emplace (username, std::make_unique <Backend::ShipHandle::Client> (handle));
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

    void BackendImpl::sendItemCallback (std::string const & sender, Direction direction) {
        for (auto & connection : connections) {
            auto & shipHandles = connection.second.shipHandles;

            if (!shipHandles.contains (sender)) {
                connections.at (sender).shipHandles.at (sender)->getSpaceshipRequest ().send().then (
                        [&] (capnp::Response <Backend::ShipHandle::GetSpaceshipResults> response) {
                            return distributeSpaceship (Spaceship (response.getSpaceship()), connection.first).then ([&] () {
                                sendItemCallback (sender, direction);
                            });
                        }).detach ([&] (kj::Exception && e) {
                            KJ_DLOG (WARNING, "Exception on establishing missing connection", e.getDescription ());
//                            doneCallback (connection.first);
                        });
//                distributeSpaceship (Spaceship (sender), connection.first).then([](){});
                return;
            }
            KJ_REQUIRE (shipHandles.contains (sender), sender, "Cannot forward directions; ship handle not found");

            auto request = shipHandles.at (sender)->sendItemRequest();
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
        log ("Erasing connection to " + sender);
        connections.erase (sender);
    }

    ::kj::Promise <void> BackendImpl::distributeSpaceship (Spaceship const & sender, std::string const & receiver) {
        log ("Distributing spaceship from " + sender.username + " to " + receiver);

        KJ_REQUIRE (connections.contains (receiver));
        KJ_REQUIRE (connections.contains (sender.username));

        auto & connection = connections.at (receiver);
        auto & shipHandles = connection.shipHandles;
        if (shipHandles.contains (sender.username)) {
            log ("Connection exists already");
            return kj::READY_NOW;
        };

        auto & registrar = connection.registrar;
        auto request = registrar.registerShipRequest();
        sender.initialise (request.initSpaceship());

        return request.send().then ([&] (capnp::Response <Backend::ShipRegistrar::RegisterShipResults> results) {
            KJ_REQUIRE (results.hasRemote());
            auto [iterator, success] = shipHandles.emplace (sender.username, std::make_unique <Backend::ShipHandle::Client> (results.getRemote()));
            KJ_ASSERT (success);
        });
    }

    void BackendImpl::broadcastSpaceship (Spaceship const & sender) {
        for (auto & connection : connections) {
            distributeSpaceship (sender, connection.first);
        }
    }
}

/* Copyright © 2022 Aaron Alef */