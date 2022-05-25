#ifndef CAPSTONE_NETWORK_BACKEND_H
#define CAPSTONE_NETWORK_BACKEND_H

#include "Network/config.h"

#include "Synchro/Synchro.h"

#include "Data/Direction.h"
#include "Data/Spaceship.h"

#include <atomic>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <ranges>

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {
    using namespace std::string_literals;

    class BackendImpl final: public Backend::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

        /// Seed of random number generator of currently running game
        std::size_t const rng_seed;

        /// List of connected clients (the things that we want to keep up to date)
        std::vector <Backend::Subscriber::Client> clients;

        /// List of connected synchros (the things this backend should synchronise with)
        std::vector <Backend::Synchro::Client> synchros;

    public:
        explicit BackendImpl (std::size_t seed);

        /** RPC function calls **/
        ::kj::Promise <void> ping    (PingContext context) override;
        ::kj::Promise <void> seed    (SeedContext context) override;
        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> join    (JoinContext context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_BACKEND_H

/* Copyright © 2022 Aaron Alef */
