#ifndef CAPSTONE_NETWORK_BACKEND_H
#define CAPSTONE_NETWORK_BACKEND_H

#include "Network/config.h"

#include "Synchro/Synchro.h"
#include "Registrar/Registrar.h"
#include "ShipSink/ShipSink.h"
#include "ShipHandle/ShipHandle.h"

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

        /// Struct holding all kinds of information about things connected
        struct Client {
            /// Whether it is a local game client or a remote synchro backend (redundant; same as maybe <synchro>)
            enum Type { LOCAL, REMOTE } type;

            /// Thing used to update the client (e.g., new ship spawned)
            Registrar_t registrar;

            /// If the client is a remote backend use this to interact with it
            kj::Maybe <Synchro_t> synchro;

            /// List of handles to all ships in possession registered by this client
            std::unordered_map <ShipName, ShipHandle_t> ships;

            /// List of all ship sinks this client needs to distribute incoming events to
            std::unordered_map <ShipName, ShipSink_t> sinks;

            Client (Registrar_t && registrar, kj::Maybe <Synchro_t> && synchro, Type type = REMOTE);
            explicit Client (Registrar_t && registrar);
        };
        /// List of all clients connected
        std::unordered_map <ClientID, Client> clients;

        /// Synchro callbacks
        kj::Own <RegistrarImpl> connectCallback (ClientID const & id, Synchro_t synchro, Registrar_t remoteRegistrar);
        kj::Promise <void> connectTo (ClientID const & id, Synchro_t synchro);

        /// RegisterShip callback
        kj::Own <ShipSinkImpl> registerShip    (Spaceship const & ship, ClientID const & id, ShipHandle_t);
        void broadcastSpaceship  (Spaceship const & ship, ClientID const & id);
        kj::Promise <void> distributeSpaceship (Spaceship const & ship, Client & receiver);

        /// ShipSink callbacks
        kj::Promise <void> doneCallback     (ShipName const & username);
        kj::Promise <void> sendItemCallback (ShipName const & username, Direction const & direction, ClientID const & id);
        kj::Promise <void> sendItemToClient (ShipName const & username, Direction const & direction, Client & receiver);

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
