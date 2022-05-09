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
#include "Callback/Callback.h"

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {
    template <class R, class... Args>
    using atomic_weak_fun = std::atomic <std::weak_ptr <std::function <R (Args...)>>>;
    typedef atomic_weak_fun <CallbackDirectionImpl::Callbacks> JoinCallback;


    class SynchroImpl final: public Synchro::Server {
    private:
        /// Seed used to initialise the game. Returned by `randomSeed`
        std::size_t const seed;

        /// List of connected clients TODO: change to map with some identifier?
        std::vector <Synchro::Callback::Client> callbacks;

        /// Callback functions for the respective RPC function calls
        JoinCallback onJoin;

        void log (std::string const & msg);

    public:
        explicit SynchroImpl (std::size_t seed);

        /// Local function calls
        void setOnJoin (JoinCallback onJoin);

        /// RPC function calls
        ::kj::Promise <void> ping       (PingContext       context) override;
        ::kj::Promise <void> randomSeed (RandomSeedContext context) override;
        ::kj::Promise <void> join       (JoinContext       context) override;
    };
} // cg

#endif //CAPSTONE_SERVER_H

/* Copyright © 2022 Aaron Alef */
