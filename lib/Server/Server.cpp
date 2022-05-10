#include "Server.h"

namespace cg {
    SynchroImpl::SynchroImpl (std::size_t seed): rng_seed {seed} {}

    SynchroImpl::Connection::Connection (CallbackHandles && handles, Synchro::ShipCallback::Client && shipCallback)
            : handles {std::move (handles)}
            , shipCallback {std::move (shipCallback)}
            {}

    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro @" << this << ": '" << msg << "'" << std::endl;
        KJ_LOG (INFO, ss.str());
    }

    ::kj::Promise <void> SynchroImpl::ping (PingContext context) {
        log ("Received Ping");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::seed (SeedContext context) {
        context.initResults().setSeed (rng_seed);
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::join (JoinContext context) {
        log ("Add new projected spaceship");
        auto params = context.getParams();
        auto result = context.getResults();

        // Check that username is unique
        // TODO: replace with UUID or ip
        std::string username = params.getUsername();
        KJ_REQUIRE (! connections.contains (username));

        // Store connection details (callback handles and 'new ship' callback)
        // TODO: figure out whether to share all connections
        auto [iterator, success] = connections.emplace (
                username, Connection ({
                    std::make_shared <std::function<void (Direction)>> (
                            [this, username] (Direction direction) { sendItemCallback (username, direction); }),
                    std::make_shared <std::function<void ()>> (
                            [this, username] () { doneCallback (username); }) // TODO: implement this
                }, params.getShipCallback()
                ));
        KJ_ASSERT (success);
        propagateItemSink (username);

        // Configure ItemSink sent back to client
        auto sink = kj::heap <ItemSinkImpl> ();
        auto handles = iterator->second.handles;
        sink->setOnSendItem (handles.sendItemCallbackHandle);
        sink->setOnDone     (handles.doneCallbackHandle);
        result.setItemSink (kj::mv (sink));

        // FIXME: connect to the other server (not needed if client connects)
        // TODO: figure out when this is needed
        // TODO: figure out whether / how to share all current connections
        // If a remote Synchro handle is attached connect back to it
//        if (params.getOther().isValue()) {
//            log ("Establish connection back");
//
//            auto client = params.getOther().getValue();
//
//            // Ensure other client uses same seed
//            client.seedRequest().send().then ([this] (capnp::Response <SeedResults> response) {
//                KJ_ASSERT (response.getSeed() != rng_seed);
//            });
//
//            auto request = client.joinRequest();
//            request.setUsername (username);
//            request.initOther().setNothing();
//            request.send();
//
//            // TODO: handle join request properly
//        }

        return kj::READY_NOW;
    }

    void SynchroImpl::sendItemCallback (std::string username, Direction direction) {
        std::for_each (connections.begin(), connections.end(),
                       [&] (std::pair<std::string const, Connection> & pair) {
            auto request = pair.second.itemSinks.at (username).sendItemRequest();
            auto request_direction = request.initItem().initDirection();
            request_direction.setAccelerate (direction.accelerate);
            request_direction.setDecelerate (direction.decelerate);
            request_direction.setRotateLeft (direction.rotateLeft);
            request_direction.setRotateRight (direction.rotateRight);
            request.send();
        });
    }
    void SynchroImpl::doneCallback (std::string username) {
        connections.erase (username);
        std::for_each (connections.begin(), connections.end(),
                       [&] (std::pair <std::string const, Connection> & pair) {
            auto & sinks = pair.second.itemSinks;
            sinks.at (username).doneRequest().send().then ([&] (...) {
                sinks.erase (username);
            });
        });
    }

    void SynchroImpl::propagateItemSink (std::string const & username) {
        std::for_each (connections.begin(), connections.end(),
                       [&, this] (std::pair <std::string const, Connection> & pair) {
                           auto & sinks = pair.second.itemSinks;
                           auto & shipCallback = connections.at (username).shipCallback;
                           auto request = shipCallback.sendSinkRequest();
                           request.setUsername (username);
                           request.send().then (
                                   [&, this] (capnp::Response <Synchro::ShipCallback::SendSinkResults> response) {
                                       sinks.emplace (username, response.getShip());
                                   });
                       });
    }
}

/* Copyright © 2022 Aaron Alef */