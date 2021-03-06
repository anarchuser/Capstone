#ifndef CAPSTONE_NETWORK_REGISTRAR_H
#define CAPSTONE_NETWORK_REGISTRAR_H

#include "Network/ShipSink/ShipSink.h"
#include "Network/ShipHandle/ShipHandle.h"

#include <functional>
#include <string>

namespace cg {
    using RegisterShipCallback = std::function <kj::Own <ShipSinkImpl> (Spaceship, ShipHandle_t)>;

    class RegistrarImpl final: public Backend::Registrar::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg) const;

        RegisterShipCallback onRegisterShip;

    public:
        /// Tell this registrar how to handle registerShip requests
        inline void setOnRegisterShip (RegisterShipCallback && callback) { onRegisterShip = std::move (callback); }

        ::kj::Promise <void> registerShip (RegisterShipContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_REGISTRAR_H

/* Copyright © 2022 Aaron Alef */
