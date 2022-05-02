#ifndef CAPSTONE_SERVER_H
#define CAPSTONE_SERVER_H

#include "config.h"

#include "lib/Server/generated/synchro.capnp.h"

#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include <iostream>
#include <functional>

#include "Direction/Direction.h"

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {

    class SynchroImpl final: public Synchro::Server {
    private:
        std::function <std::function <void (Direction)> ()> onNewRemoteSpaceship;

        void log (std::string msg);
        static std::function <void (Direction direction)> updateDirectionCallback;

    public:
        explicit SynchroImpl (std::function <std::function <void (Direction)> ()> && onNewStreamCallback);

        ::kj::Promise <void> connect (ConnectContext context) override;

        ::kj::Promise <void> updateDirection (UpdateDirectionContext context) override;

        ::kj::Promise <void> streamDirections (StreamDirectionsContext context) override;
    };

    class DirectionCallbackImpl final: public Synchro::DirectionCallback::Server {
    private:
        void log (std::string msg);

        std::function <void (Direction)> updateDirectionCallback;

    public:
        DirectionCallbackImpl (std::function <void (Direction)> && updateDirectionCallback);

        ::kj::Promise <void> sendDirection (SendDirectionContext context) override;
        ::kj::Promise <void> done (DoneContext context) override;
    };
} // cg

#endif //CAPSTONE_SERVER_H

/* Copyright © 2022 Aaron Alef */
