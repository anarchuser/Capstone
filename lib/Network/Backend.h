#ifndef CAPSTONE_NETWORK_BACKEND_H
#define CAPSTONE_NETWORK_BACKEND_H

#include "Network/config.h"

#include "Data/Direction.h"
#include "Data/Spaceship.h"
#include "ShipHandle/ShipHandle.h"
#include "ShipRegistrar/ShipRegistrar.h"

#include <atomic>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <unordered_map>

#include <ranges>

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {
    class BackendImpl final: public Backend::Server {
    private:

        /// Handles to all spaceships registered to the server
        //TODO: replace username with proper UUID
        std::unordered_map <std::string, Backend::ShipHandle::Client> shipHandles;

        /// List of connected game clients
        std::vector <Backend::ShipRegistrar::Client> registrars;

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

        /// RPC function calls
        ::kj::Promise <void> ping (PingContext context) override;
        ::kj::Promise <void> seed (SeedContext context) override;
        ::kj::Promise <void> registerClient (RegisterClientContext context) override;
        ::kj::Promise <void> registerShip (RegisterShipContext context) override;
        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> requestConnect (RequestConnectContext context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_BACKEND_H

/* Copyright © 2022 Aaron Alef */
