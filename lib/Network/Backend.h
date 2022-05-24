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
        void log (std::string const & msg);

        struct Connection {
            /// Registrar to send new ship registered requests to
            Backend::ShipRegistrar::Client registrar;

            /// Handles to all spaceships registered to the server
            std::unordered_map <std::string, Backend::ShipHandle::Client> shipHandles;
        };

        /// List of everything that demands knowledge of every ship there is
        std::unordered_map <std::string, Connection> connections;

        /// Seed used to initialise the game. Returned by `randomSeed`
        std::size_t const rng_seed;

        /// Identifier of this backend
        std::string const name;

        /// Callback to execute when any ship updates their directions
        void sendItemCallback (std::string const & sender, Direction direction);
        /// Callback to execute when any ship got destroyed
        void doneCallback     (std::string const & sender);

        /// Callback to call when our registrar received a new spaceship
        kj::Own <ShipHandleImpl> registerShipCallback (Spaceship const & spaceship, Backend::ShipHandle::Client handle);
        /// Insert the remote registrar handle into list of connections and return a registrar of our own
        kj::Own <ShipRegistrarImpl> exchangeRegistrars (std::string const & name, Backend::ShipRegistrar::Client remote);

        /// Distribute spaceship to one connection
        void distributeSpaceship (Spaceship const & sender, std::string const & receiver);
        /// Distribute spaceship to every connection
        void broadcastSpaceship (Spaceship const & sender);

    public:
        explicit BackendImpl (std::size_t seed, std::string name);

        /** RPC function calls **/
        ::kj::Promise <void> ping (PingContext context) override;
        ::kj::Promise <void> seed (SeedContext context) override;
        ::kj::Promise <void> registerClient (RegisterClientContext context) override;
        ::kj::Promise <void> synchro (SynchroContext context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_BACKEND_H

/* Copyright © 2022 Aaron Alef */
