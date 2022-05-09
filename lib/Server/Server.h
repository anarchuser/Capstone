#ifndef CAPSTONE_SERVER_H
#define CAPSTONE_SERVER_H

#include "config.h"

#include "lib/Server/generated/synchro.capnp.h"

#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>

#include "Direction/Direction.h"
#include "Callback/ItemSink.h"
#include "Callback/ShipCallback.h"

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

#define MAX_CONNECTIONS 256

namespace cg {
    class SynchroImpl final: public Synchro::Server {
    private:

        struct Connection {
            std::string username;
        };

        /// List of open connections TODO: check why std::vector causes problems for kj::heap
        std::vector <Connection> connections;
//        cg::ShipCallbackImpl callbacks [MAX_CONNECTIONS];
//        std::size_t index = 0;

        /// Seed used to initialise the game. Returned by `randomSeed`
        std::size_t const rng_seed;

        void log (std::string const & msg);

    public:
        explicit SynchroImpl (std::size_t seed);

        /// RPC function calls
        ::kj::Promise <void> ping (PingContext context) override;
        ::kj::Promise <void> seed (SeedContext context) override;
        ::kj::Promise <void> join (JoinContext context) override;
    };
} // cg

#endif //CAPSTONE_SERVER_H

/* Copyright © 2022 Aaron Alef */
