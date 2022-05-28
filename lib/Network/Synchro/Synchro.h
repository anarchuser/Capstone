#ifndef CAPSTONE_NETWORK_SYNCHRO_H
#define CAPSTONE_NETWORK_SYNCHRO_H

#include "Network/Backend.h"

#include "Network/Registrar/Registrar.h"

#include <functional>

namespace cg {
    class RegistrarImpl;
    using ConnectCallback = std::function <kj::Own <RegistrarImpl> (std::string const &, Backend::Synchro::Client, Backend::Registrar::Client)>;

    class SynchroImpl final: public Backend::Synchro::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

        ConnectCallback onConnect;

        std::string const ID;

    public:
        inline explicit SynchroImpl (std::string id): ID {id} {}

        inline void setOnConnect (ConnectCallback && callback) { onConnect = callback; }

        ::kj::Promise <void> connect (ConnectContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */
