#ifndef CAPSTONE_NETWORK_SYNCHRO_H
#define CAPSTONE_NETWORK_SYNCHRO_H

#include "Network/config.h"

namespace cg {

    class SynchroImpl final: public Backend::Synchro::Server {
    private:
        void log (std::string const & msg);

    public:
        kj::Promise <void> connect (ConnectContext context) override;
        kj::Promise <void> disconnect (DisconnectContext context) override;
        kj::Promise <void> sendShip (SendShipContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */
