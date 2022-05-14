#include "Server.h"

namespace cg {
    SynchroImpl::SynchroImpl (std::size_t seed): rng_seed {seed} {}

    SynchroImpl::Connection::Connection (CallbackHandles && handles, Synchro::ShipCallback::Client && shipCallback)
            : handles {std::move (handles)}
            , shipCallback {std::move (shipCallback)}
            {}

    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    ::kj::Promise <void> SynchroImpl::ping (PingContext context) {
        log ("Ping");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::seed (SeedContext context) {
        log ("RNG Seed (" + std::to_string (rng_seed) + ") requested");
        context.initResults().setSeed (rng_seed);
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::join (JoinContext context) {
        auto params = context.getParams();
        auto param_ship = params.getSpaceship();
        auto result = context.getResults();

        Spaceship spaceship = {
                param_ship.getUsername(), {
                    param_ship.getPosition().getX(),
                    param_ship.getPosition().getY()
                }, {
                    param_ship.getVelocity().getX(),
                    param_ship.getVelocity().getY(),
                },
                param_ship.getAngle()
        };

        // Check that username is unique
        // TODO: replace with UUID or ip
        std::string const & username = params.getSpaceship().getUsername();
        KJ_REQUIRE (! connections.contains (username), username, "Username already in use");
        log (std::string ("Join request received from user ") + username);

        // Store connection details (callback handles and 'new ship' callback)
        // TODO: figure out whether to share all connections
        auto [iterator, success] = connections.emplace (username, Connection (
                { [this, username] (Direction direction) {
                    sendItemCallback (username, direction); }
                , [this, username] () {
                    doneCallback (username);
                }}, params.getShipCallback()));
        KJ_ASSERT (success);
        broadcastSpaceship (spaceship);

        // Configure ItemSink sent back to client
        log ("Configure ItemSink result");
        auto sink = kj::heap <ItemSinkImpl> ();
        auto handles = iterator->second.handles;
        sink->setOnSendItem (handles.sendItemCallbackHandle);
        sink->setOnDone     (handles.doneCallbackHandle);
        result.setItemSink (kj::mv (sink));

        // FIXME: connect to the other server (not needed if client connects)
        // TODO: figure out when this is needed
        // TODO: figure out whether / how to share all current connections
        // If a remote Synchro handle is attached connect back to it
        if (params.getOther().isValue()) {
            log ("Establish connection back");

            // Ensure other client uses same seed
            auto client = params.getOther().getValue();
            client.seedRequest().send().then ([&] (capnp::Response <SeedResults> response) {
                KJ_ASSERT (response.getSeed() != rng_seed);

                auto request = client.joinRequest();
                auto request_ship = request.initSpaceship();
                request_ship.setUsername (username);
                // TODO: set real spaceship position + velocity (currently defaults to 0.0)
                request.initOther().setNothing();
                request.send();

                // TODO: handle join request properly
            });
        }

        return kj::READY_NOW;
    }

    void SynchroImpl::sendItemCallback (std::string sender, Direction direction) {
        std::for_each (connections.begin(), connections.end(),
                       [&] (std::pair<std::string const, Connection> & pair) {
            auto & receiver = pair.first;
            auto & sinks = pair.second.itemSinks;

            if (! sinks.contains (sender)) {
                distributeSpaceship ({sender}, receiver);
            }

            KJ_REQUIRE (sinks.contains (sender));
            auto request = sinks.at (sender).sendItemRequest();
            auto request_direction = request.initItem().initDirection();

            request_direction.setAccelerate (direction.accelerate);
            request_direction.setDecelerate (direction.decelerate);
            request_direction.setRotateLeft (direction.rotateLeft);
            request_direction.setRotateRight (direction.rotateRight);

            request.send();
        });
    }
    void SynchroImpl::doneCallback (std::string username) {
        log ("Closing sinks from " + username);

        connections.erase (username);
        std::for_each (connections.begin(), connections.end(),
                       [&] (std::pair <std::string const, Connection> & pair) {
            auto & sinks = pair.second.itemSinks;
            if (sinks.contains (username)) {
                sinks.at (username).doneRequest ().send ()
                .then ([&] (...) {
                    sinks.erase (username);
                });
            }
        });
    }

    void SynchroImpl::distributeSpaceship (Spaceship const & sender, std::string const & receiver) {
        log ("Relay directions from " + sender.username += " to " + receiver);

        KJ_REQUIRE (connections.contains (sender.username));
        KJ_REQUIRE (connections.contains (receiver));

        if (connections.at (receiver).itemSinks.contains (sender.username)) {
            log ("Connection exists already");
            return;
        }

        auto & shipCallback = connections.at (receiver).shipCallback;
        auto & sinks = connections.at (receiver).itemSinks;
        auto request = shipCallback.sendSinkRequest ();
        {
            auto ship = request.initSpaceship();
            ship.setUsername (sender.username);
            auto pos = ship.initPosition();
            pos.setX (sender.position[0]);
            pos.setY (sender.position[1]);
            auto vel = ship.initVelocity();
            vel.setX (sender.velocity[0]);
            vel.setY (sender.velocity[1]);
            ship.setAngle (sender.angle);
        }
        sinks.emplace (sender.username, request.send().getShip());
    }

    void SynchroImpl::broadcastSpaceship (Spaceship const & sender) {
        std::for_each (connections.begin(), connections.end(),
                       [&, this] (std::pair <std::string const, Connection> & pair) {
            distributeSpaceship (sender, pair.first);
        });
    }
}

/* Copyright © 2022 Aaron Alef */