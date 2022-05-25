#ifndef CAPSTONE_NETWORK_REGISTRAR_H
#define CAPSTONE_NETWORK_REGISTRAR_H

#include "Network/Backend.h"

namespace cg {

class RegistrarImpl final: public Backend::Registrar::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

    public:
    };

} // cg

#endif //CAPSTONE_NETWORK_REGISTRAR_H

/* Copyright © 2022 Aaron Alef */
