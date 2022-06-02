#ifndef CAPSTONE_NETWORK_SYNCHRO_H
#define CAPSTONE_NETWORK_SYNCHRO_H

#include "Network/Registrar/Registrar.h"
#include "Network/ShipSink/ShipSink.h"
#include "Network/ShipHandle/ShipHandle.h"

#include "Data/Client.h"

#include <functional>
#include <optional>

namespace cg {
    class RegistrarImpl;
    using ConnectCallback = std::function <kj::Own <RegistrarImpl> (ClientID const &, Synchro_t, Registrar_t)>;
    using ShareCallback   = std::function <kj::Promise <void> (ClientID const &, Synchro_t)>;

    class SynchroImpl final: public Backend::Synchro::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

        ConnectCallback onConnect;
        ShareCallback onShare;

        ClientID const & ID;

        std::optional <LocalClient> & local;                  /// The one allowed *local* client connected
        std::unordered_map <ClientID, RemoteClient> & remote; /// List of all *remote* clients connected

    public:
        SynchroImpl (ClientID const & id, std::optional <LocalClient> & local, std::unordered_map <ClientID, RemoteClient> & remote);

        inline void setOnConnect (ConnectCallback && callback) { onConnect = callback; }
        inline void setOnShare   (ShareCallback   && callback) { onShare   = callback; }

        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> share (ShareContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */
