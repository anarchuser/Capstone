#ifndef CAPSTONE_NETWORK_SYNCHRO_H
#define CAPSTONE_NETWORK_SYNCHRO_H

#include "Network/config.h"

#include "Network/ShipRegistrar/ShipRegistrar.h"

#include <functional>

namespace cg {
    using ConnectCallback = std::function <void (std::string const &, Backend::Synchro::Client)>;
    using SyncCallback = std::function <kj::Own <ShipRegistrarImpl> (std::string const &, Backend::ShipRegistrar::Client)>;

    class SynchroImpl final: public Backend::Synchro::Server {
    private:
        ConnectCallback onConnect;
        SyncCallback onSync;

        void log (std::string const & msg);

    public:
        void setOnConnect (ConnectCallback && onConnect);
        void setOnSync (SyncCallback && onSync);

        ::kj::Promise <void> connect (ConnectContext context) override;
        ::kj::Promise <void> sync (SyncContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */
