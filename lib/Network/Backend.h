#ifndef CAPSTONE_NETWORK_BACKEND_H
#define CAPSTONE_NETWORK_BACKEND_H

#include "Network/config.h"

#include "Data/Direction.h"
#include "Data/Spaceship.h"
#include "Data/Address.h"
#include "ShipHandle/ShipHandle.h"
#include "ShipRegistrar/ShipRegistrar.h"
#include "Synchro/Synchro.h"

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
    class BackendImpl final: public Backend::Server {
    private:
        /// List of connected game clients
        std::unique_ptr <Backend::ShipRegistrar::Client> gameClient;

        /// Handles to all spaceships registered to the server
        std::unordered_map <std::string, Backend::ShipHandle::Client> shipHandles;

        /// Map address -> remote server connection
        std::unordered_map <std::string, Backend::Synchro::Client> connections;

        /// Seed used to initialise the game. Returned by `randomSeed`
        std::size_t const rng_seed;

        void sendItemCallback (std::string const & sender, Direction direction);
        void doneCallback     (std::string username);

        void broadcastSpaceship  (Spaceship const & sender);

        kj::Own <ShipHandleImpl> registerShipCallback (Spaceship const & spaceship, Backend::ShipHandle::Client handle);

        void log (std::string const & msg);

    public:
        explicit BackendImpl (std::size_t seed);
        ~BackendImpl();

        /// RPC function calls
        ::kj::Promise <void> ping (PingContext context) override;
        ::kj::Promise <void> seed (SeedContext context) override;
        ::kj::Promise <void> registerClient (RegisterClientContext context) override;
        ::kj::Promise <void> synchro (SynchroContext context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_BACKEND_H

/* Copyright © 2022 Aaron Alef */
