#include "Connection.h"

namespace kt {
    Connection::Connection (Direction const * direction, std::string address, unsigned short port):
            direction {direction},
            rpcClient {kj::heap <capnp::EzRpcClient> (address, port)},
            client {rpcClient->getMain<Synchro>()},
            waitscope {rpcClient->getWaitScope()},
            callback {client.streamDirectionsRequest().send().wait (waitscope).getCallback()} {}

    Connection::~Connection () noexcept {
        callback.doneRequest ().send().wait (waitscope);
        waitscope.cancelAllDetached();
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

    void Connection::ping () {
        client.connectRequest ().send().wait (waitscope);
    }
}

/* Copyright © 2022 Aaron Alef */
