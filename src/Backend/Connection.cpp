#include "Connection.h"

namespace kt {
    Connection::Connection (std::function <cg::DirectionCallback ()> & onStreamDirections, std::size_t seed, Direction const * direction, std::string address, unsigned short port):
            seed {seed},
            direction {direction},
            rpcClient {kj::heap <capnp::EzRpcClient> (address, port)},
            client {rpcClient->getMain<Synchro>()},
            waitscope {rpcClient->getWaitScope()},
            callback {initCallback (onStreamDirections)} {}

    Connection::~Connection () noexcept {
        callback.doneRequest ().send().wait (waitscope);
    }

    void Connection::update () {
        auto request = callback.sendDirectionRequest ();

        auto request_direction = request.initDirection();
        request_direction.setAccelerate (direction->accelerate);
        request_direction.setDecelerate (direction->decelerate);
        request_direction.setRotateLeft (direction->rotateLeft);
        request_direction.setRotateRight (direction->rotateRight);

        request.send().wait (waitscope);
    }

    Synchro::DirectionCallback::Client
    Connection::initCallback (std::function<cg::DirectionCallback ()> & onStreamDirections) {
        auto request = client.streamDirectionsRequest ();
        auto impl = kj::heap <cg::SynchroImpl> (seed, std::forward <std::function <cg::DirectionCallback ()>> (onStreamDirections));
        request.initClient().setValue (kj::mv (impl));
        return request.send().wait (waitscope).getCallback();
    }

    bool Connection::ping (std::string const & ip, short port) {
        auto client = capnp::EzRpcClient (ip, port);
        try {
            client.getMain <Synchro> ().connectRequest ().send().wait (client.getWaitScope());
            return true;
        } catch (...) {
            logs::warning ("Failed to connect to '%s:%d'", ip.c_str(), port);
        }
        return false;
    }
}

/* Copyright © 2022 Aaron Alef */
