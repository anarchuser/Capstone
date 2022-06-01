#ifndef CAPSTONE_NETWORK_SYNCHRO_H
#define CAPSTONE_NETWORK_SYNCHRO_H

#include "Network/Registrar/Registrar.h"

#include <functional>

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

        ClientID const ID;

    public:
        inline explicit SynchroImpl (ClientID id): ID {id} {}

        inline void setOnConnect (ConnectCallback && callback) { onConnect = callback; }
        inline void setOnShare   (ShareCallback   && callback) { onShare   = callback; }

        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> share (ShareContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */
