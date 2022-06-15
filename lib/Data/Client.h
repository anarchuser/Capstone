#ifndef CAPSTONE_NETWORK_CLIENT_H
#define CAPSTONE_NETWORK_CLIENT_H

#include "Network/config.h"

#include <unordered_map>
#include <thread>

namespace cg {
    /// This struct holds everything necessary to manage one connected client
    struct Client {
        /// Construct a new client reference to the given Registrar
        inline explicit Client (Registrar_t && registrar): registrar {std::move (registrar)} {}

        /// This registrar is used to tell the referenced client of new ships registered
        Registrar_t registrar;

        /// List of handles to all ships this client has registered and therefore owns
        std::unordered_map <ShipName, ShipHandle_t> ships;

        /// List of all known ship sinks to redirect
        std::unordered_map <ShipName, ShipSink_t> sinks;

        /// Close connection to the given ship. Send a done request and delete its sink and handle, if available
        kj::Promise<void> erase (ShipName const & username);

        /// Close connection to all registered ships
        kj::Promise<void> destroy();
    };

    /// A client that is connected directly, not via a remote Synchro instance
    struct LocalClient : public Client {
        /// Construct a new local client instance
        inline explicit LocalClient (Registrar_t && registrar): Client (std::forward <Registrar_t> (registrar)) {}
    };

    /// A client additionally holding a reference to a remote synchro
    struct RemoteClient : public Client {
        /// Construct a new remote client connected to the given remote Synchro instance
        inline RemoteClient (Registrar_t && registrar, Synchro_t && synchro)
                : Client (std::forward <Registrar_t> (registrar))
                , synchro {std::move (synchro)} {}

        /// The remote backend through which this client is accessed
        Synchro_t synchro;
    };

} // cg

#endif //CAPSTONE_NETWORK_CLIENT_H

/* Copyright © 2022 Aaron Alef */
