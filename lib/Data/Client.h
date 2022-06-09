#ifndef CAPSTONE_NETWORK_CLIENT_H
#define CAPSTONE_NETWORK_CLIENT_H

#include "Network/config.h"

#include <unordered_map>
#include <thread>

namespace cg {
    /// Struct holding all kinds of information about things connected
    struct Client {
        inline explicit Client (Registrar_t registrar): registrar {std::move (registrar)} {}

        /// Send a done request to the sink of the given ship, then remove it from the list
        kj::Promise<void> erase (ShipName const & username);

        /// Ensure every sink has been deleted
        kj::Promise<void> destroy();

        /// Thing used to update the client (e.g., new ship spawned)
        Registrar_t registrar;

        /// List of handles to all ships in possession registered by this client
        std::unordered_map <ShipName, ShipHandle_t> ships;

        /// List of all ship sinks this client needs to distribute incoming events to
        std::unordered_map <ShipName, ShipSink_t> sinks;
    };

    /// Effectively the same as its parent; indicates a local client
    struct LocalClient : public Client {
        /// Constructs a new local client instance
        inline explicit LocalClient(Registrar_t registrar): Client (registrar) {}
    };

    /// A client additionally holding a reference to a remote synchro
    struct RemoteClient : public Client {
        /// Constructs a new remote client connected to the given synchro instance
        inline RemoteClient (Registrar_t registrar, Synchro_t synchro): Client (registrar), synchro {std::move (synchro)} {}

        /// If the client is a remote backend use this to interact with it
        Synchro_t synchro;
    };

} // cg

#endif //CAPSTONE_NETWORK_CLIENT_H

/* Copyright © 2022 Aaron Alef */
