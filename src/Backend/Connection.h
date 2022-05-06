#ifndef CAPSTONE_CONNECTION_H
#define CAPSTONE_CONNECTION_H

#include "config.h"

#include "Direction/Direction.h"

#include "Server/generated/synchro.capnp.h"
#include "Server/Server.h"
#include <capnp/ez-rpc.h>
#include <kj/debug.h>

namespace kt {

    /// Wrapper class around streamDirection requests TODO: renmame
    class Connection {
    private:
        Direction const * direction;
        std::size_t seed;

        kj::Own <capnp::EzRpcClient> rpcClient;
        Synchro::Client client;
        kj::WaitScope & waitscope;
        Synchro::DirectionCallback::Client callback;

        Synchro::DirectionCallback::Client initCallback (std::function <cg::DirectionCallback ()> & onStreamDirections);

    public:
        Connection (std::function <cg::DirectionCallback ()> & onStreamDirections,
                    std::size_t seed,
                    Direction const * direction,
                    std::string address,
                    unsigned short port);
        ~Connection() noexcept;

        void update ();
        void ping ();
    };

} // kt

#endif //CAPSTONE_CONNECTION_H

/* Copyright © 2022 Aaron Alef */
