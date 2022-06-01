#ifndef CAPSTONE_NETWORK_CLIENT_H
#define CAPSTONE_NETWORK_CLIENT_H

#include "Network/config.h"

#include <unordered_map>

namespace cg {
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

} // cg

#endif //CAPSTONE_NETWORK_CLIENT_H

/* Copyright © 2022 Aaron Alef */
