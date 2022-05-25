#ifndef CAPSTONE_NETWORK_SYNCHRO_H
#define CAPSTONE_NETWORK_SYNCHRO_H

#include "Network/Backend.h"

namespace cg {

    class SynchroImpl final: Backend::Synchro::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

    };

} // cg

#endif //CAPSTONE_NETWORK_SYNCHRO_H

/* Copyright © 2022 Aaron Alef */
