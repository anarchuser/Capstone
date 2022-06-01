#ifndef CAPSTONE_NETWORK_CLIENT_H
#define CAPSTONE_NETWORK_CLIENT_H

#include "Network/config.h"

#include <unordered_map>

namespace cg {
    /// Struct holding all kinds of information about things connected
    struct Client {
        inline explicit Client (Registrar_t registrar): registrar {registrar} {}

        kj::Promise <void> erase (ShipName const & username);

        /// Thing used to update the client (e.g., new ship spawned)
        Registrar_t registrar;

        /// List of handles to all ships in possession registered by this client
        std::unordered_map <ShipName, ShipHandle_t> ships;

        /// List of all ship sinks this client needs to distribute incoming events to
        std::unordered_map <ShipName, ShipSink_t> sinks;
    };

    struct LocalClient : public Client {
        inline explicit LocalClient(Registrar_t registrar): Client (registrar) {}
    };

    struct RemoteClient : public Client {
        inline RemoteClient (Registrar_t registrar, Synchro_t synchro): Client (registrar), synchro {synchro} {}

        /// If the client is a remote backend use this to interact with it
        Synchro_t synchro;
    };

} // cg

#endif //CAPSTONE_NETWORK_CLIENT_H

/* Copyright © 2022 Aaron Alef */
