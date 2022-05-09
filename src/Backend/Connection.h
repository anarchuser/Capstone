#ifndef CAPSTONE_CONNECTION_H
#define CAPSTONE_CONNECTION_H

#include "config.h"

#include "Direction/Direction.h"

#include "Server/generated/synchro.capnp.h"
#include "Server/Server.h"
#include <capnp/ez-rpc.h>
#include <kj/debug.h>

#include "oxygine-framework.h"

namespace kt {
    using namespace oxygine;

    /// Wrapper class around streamDirection requests TODO: renmame
    class Connection {
    private:
        Direction const * direction;
        std::size_t seed;

        kj::Own <capnp::EzRpcClient> rpcClient;
        Synchro::Client client;
        kj::WaitScope & waitscope;

        void initCallback (cg::JoinCallback & onJoin);

    public:
        Connection (cg::JoinCallback & onStreamDirections,
                    std::size_t seed,
                    Direction const * direction,
                    std::string address,
                    unsigned short port);
        ~Connection() noexcept;

        static bool ping (std::string const & ip, short port);
    };

} // kt

#endif //CAPSTONE_CONNECTION_H

/* Copyright © 2022 Aaron Alef */
