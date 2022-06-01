#ifndef CAPSTONE_NETWORK_BACKEND_H
#define CAPSTONE_NETWORK_BACKEND_H

#include "Network/config.h"

#include "Synchro/Synchro.h"
#include "Registrar/Registrar.h"
#include "ShipSink/ShipSink.h"
#include "ShipHandle/ShipHandle.h"

#include "Data/Direction.h"
#include "Data/Spaceship.h"
#include "Data/Client.h"

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

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

namespace cg {
    using namespace std::string_literals;

    class BackendImpl final: public Backend::Server {
    private:
        /// Seed of random number generator of currently running game
        std::size_t const rng_seed;

        /// Own identifier
        ClientID const ID;

        /// List of all clients connected
        std::unordered_map <ClientID, Client> clients;

        /// Synchro callbacks
        kj::Own <RegistrarImpl> connectCallback (ClientID const & id, Synchro_t synchro, Registrar_t remoteRegistrar);
        kj::Promise <void> connectTo (ClientID const & id, Synchro_t synchro);
        void shareConnections (ClientID const & id, Synchro_t synchro);

        /// RegisterShip callback
        kj::Own <ShipSinkImpl> registerShip    (Spaceship const & ship, ClientID const & id, ShipHandle_t);
        void broadcastSpaceship  (Spaceship const & ship, ClientID const & id);
        kj::Promise <void> distributeSpaceship (Spaceship const & ship, Client & receiver);

        /// ShipSink callbacks
        kj::Promise <void> doneCallback     (ShipName const & username);
        kj::Promise <void> sendItemCallback (ShipName const & username, Direction const & direction, ClientID const & id);
        kj::Promise <void> sendItemToClient (ShipName const & username, Direction const & direction, Client & receiver);

        /// Callback builder functions
        kj::Own <RegistrarImpl> newRegistrar (ClientID const & id);
        kj::Own <SynchroImpl>   newSynchro   (ClientID const & id);

        /// Disconnect the client with this id
        void disconnect (ClientID const & id);

        /// Log function of this implementation
        void log (std::string const & msg);

    public:
        explicit BackendImpl (std::size_t seed, ClientID id);

        /** RPC function calls **/
        ::kj::Promise <void> ping    (PingContext    context) override;
        ::kj::Promise <void> seed    (SeedContext    context) override;
        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> join    (JoinContext    context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_BACKEND_H

/* Copyright © 2022 Aaron Alef */
