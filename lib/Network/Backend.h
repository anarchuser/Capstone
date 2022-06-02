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

namespace cg {
    using namespace std::string_literals;

    class BackendImpl final: public Backend::Server {
    private:
        /* Member variables */
        ClientID const ID;                                  /// Our own identifier to send other backends
        std::size_t const rng_seed;                         /// Seed for the random number generator
        std::optional <LocalClient> local;                  /// The one allowed *local* client connected
        std::unordered_map <ClientID, RemoteClient> remote; /// List of all *remote* clients connected
        SynchroImpl synchro;
        // TODO: do we need a whole map for local clients?

        /// Return raw pointer to Client if id was in remote or local. Nullptr otherwise
        Client * findClient (ClientID const & id);

        /* Client connection functions */
        /// Callback for calls to Synchro::connect
        kj::Own <RegistrarImpl> connectCallback (ClientID const & id, Synchro_t synchro, Registrar_t remoteRegistrar);
        /// Share all our remote clients with the given synchro
        void shareConnections (ClientID const & id, Synchro_t synchro);
        /// Connect to a synchro and store the resulting new client
        kj::Promise <void> connectTo (ClientID const & id, Synchro_t synchro);

        /* Ship registration functions */
        /// Configure the newly registered spaceship and return a sink to it. Registrar callback
        kj::Own <ShipSinkImpl> registerShip    (Spaceship const & ship, ClientID const & id, ShipHandle_t);
        /// Distribute spaceship to every client
        void broadcastSpaceship  (Spaceship const & ship, ShipHandle_t handle, bool isRemote);
        /// Hand out spaceship to the specified client
        kj::Promise <void> distributeSpaceship (Spaceship const & ship, ShipHandle_t handle, Client & receiver);

        /* ShipSink callback functions */
        /// Callback on closing sink; propagate to sink of all clients
        void doneCallback     (ShipName const & username);
        /// Callback on item received; propagate to sink of all clients
        void sendItemCallback (Direction const & direction, Spaceship const & data, ClientID const & id);
        /// Helper to propagate item to sink of one specific client
        kj::Promise <void> sendItemToClient (Direction const & direction, Spaceship const & data, ShipHandle_t handle, Client & receiver);

        /* Impl builder functions */
        /// Build a new Registrar with registerShip as callback
        kj::Own <RegistrarImpl> newRegistrar (ClientID const & id);
        /// Build a new Synchro with corresponding callbacks
        kj::Own <SynchroImpl>   newSynchro   (ClientID const & id);

        /// Disconnect the client with this id, if exists
        kj::Promise <void> disconnect (ClientID const & id);

        /// Log function of this implementation
        void log (std::string const & msg);

    public:
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
