#ifndef CAPSTONE_NETWORK_REGISTRAR_H
#define CAPSTONE_NETWORK_REGISTRAR_H

#include "Network/Backend.h"

#include <functional>

namespace cg {
    using RegisterShipCallback = std::function <void ()>;

    class RegistrarImpl final: public Backend::Registrar::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

        RegisterShipCallback onRegisterShip;

    public:
        void setOnRegisterShip (RegisterShipCallback && callback);

        ::kj::Promise <void> registerShip (RegisterShipContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_REGISTRAR_H

/* Copyright © 2022 Aaron Alef */
