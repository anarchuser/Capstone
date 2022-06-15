#ifndef CAPSTONE_NETWORK_BACKEND_H
#define CAPSTONE_NETWORK_BACKEND_H

#include "Network/config.h"

#include "Synchro/Synchro.h"
#include "Registrar/Registrar.h"
#include "ShipSink/ShipSink.h"
#include "ShipHandle/ShipHandle.h"
#include "Client.h"

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
#include <iterator>

namespace cg {
    using namespace std::string_literals;

    /// Implementation of the Backend::Server interface
    class BackendImpl final: public Backend::Server {
    private:
        /// Our own identifier to send other backends
        ClientID const ID;
        /// Seed for the random number generator
        std::size_t const rng_seed;
        /// The one allowed *local* client connected
        std::optional <LocalClient> local;
        /// List of all *remote* clients connected
        std::unordered_map <ClientID, RemoteClient> remotes;
        /// Our own synchro instance handling game synchronisation
        SynchroImpl synchro;

        /// Log function of this implementation
        void log (std::string const & msg) const;

    public:
        /// Construct a new Backend::Server implementation
        BackendImpl (std::size_t seed, ClientID id);

        /* RPC function call implementations */
        ::kj::Promise <void> ping    (PingContext    context) override;
        ::kj::Promise <void> seed    (SeedContext    context) override;
        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> join    (JoinContext    context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_BACKEND_H

/* Copyright © 2022 Aaron Alef */
