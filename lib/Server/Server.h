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
#include "Callback/DirectionCallback.h"

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {

    class SynchroImpl final: public Synchro::Server {
    private:
        std::function <cg::DirectionCallback ()> onStreamDirections;

        void log (std::string const & msg);

    public:
        explicit SynchroImpl (std::function <DirectionCallback ()> && onStreamDirections);

        ::kj::Promise <void> connect (ConnectContext context) override;

        ::kj::Promise <void> streamDirections (StreamDirectionsContext context) override;
    };
} // cg

#endif //CAPSTONE_SERVER_H

/* Copyright © 2022 Aaron Alef */
