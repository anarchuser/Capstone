#ifndef CAPSTONE_NETWORK_SYNCHRO_H
#define CAPSTONE_NETWORK_SYNCHRO_H

#include "Network/config.h"

#include "Network/ShipRegistrar/ShipRegistrar.h"

#include <functional>

namespace cg {
    using ConnectCallback = std::function <void ()>;

    class SynchroImpl final: public Backend::Synchro::Server {
    private:
        ConnectCallback onConnect;

        void log (std::string const & msg);

    public:
        void setOnConnect (ConnectCallback && onConnect);

        ::kj::Promise <void> connect (ConnectContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */
