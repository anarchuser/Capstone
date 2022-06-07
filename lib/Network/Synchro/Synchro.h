#ifndef CAPSTONE_NETWORK_SYNCHRO_H
#define CAPSTONE_NETWORK_SYNCHRO_H

#include "Network/Registrar/Registrar.h"
#include "Network/ShipSink/ShipSink.h"
#include "Network/ShipHandle/ShipHandle.h"

#include "Data/Client.h"
#include "Data/Item.h"

#include <functional>
#include <optional>

namespace cg {
    using namespace std::string_literals;

    /// Implementation of the Synchro::Server interface
    class SynchroImpl final: public Backend::Synchro::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

        /// The identifier of this instance
        ClientID const ID;

        std::optional <LocalClient> & local;                   /// The one allowed *local* client connected
        std::unordered_map <ClientID, RemoteClient> & remotes; /// List of all *remotes* clients connected

        /// Return raw pointer to Client if id was in remote or local. Nullptr otherwise
        Client * findClient (ClientID const & id);
        /// Disconnect the client with this id, if exists
        kj::Promise <void> disconnect (ClientID const & id);

        /* Ship registration functions */
        /// Configure the newly registered spaceship and return a sink to it. Registrar callback
        kj::Own <ShipSinkImpl> registerShip (Spaceship const & ship, ClientID const & id, ShipHandle_t);
        /// Distribute spaceship to every client
        void broadcastSpaceship (Spaceship const & ship, ShipHandle_t handle, bool isRemote);
        /// Hand out spaceship to the specified client
        kj::Promise <void> distributeSpaceship (Spaceship const & ship, ShipHandle_t handle, Client & receiver);

        /* ShipSink callback functions */
        /// Callback on closing sink; propagate to sink of all clients
        void doneCallback (ShipName const & username);
        /// Callback on item received; propagate to sink of all clients
        void sendItemCallback (Item const & item, ClientID const & id);
        /// Helper to propagate item to sink of one specific client
        kj::Promise <void> sendItemToClient (Item const & item, ShipHandle_t handle, Client & receiver);

        /// Compare original ship to all remote clones and estimate their true status
        kj::Promise <Spaceship> estimateShipData (Spaceship const & original);

    public:
        /// Constructs a new Synchro::Server implementation
        SynchroImpl (ClientID id, std::optional <LocalClient> & local, std::unordered_map <ClientID, RemoteClient> & remotes);

        /* Impl builder functions */
        /// Build a new Registrar with registerShip as callback
        kj::Own <RegistrarImpl> newRegistrar (ClientID const & id);
        /// Build a new Synchro with corresponding callbacks
        kj::Own <SynchroImpl>   newSynchro   (ClientID const & id);

        /* Client connection functions */
        /// Share all our remote clients with the given remote
        void shareConnections (ClientID const & id, Synchro_t remote);
        /// Connect to a remote and store the resulting new client
        kj::Promise <void> connectTo (ClientID const & id, Synchro_t remote);

        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> share   (ShareContext   context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */
