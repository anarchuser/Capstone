#ifndef CAPSTONE_NETWORK_SHIPREGISTRAR_H
#define CAPSTONE_NETWORK_SHIPREGISTRAR_H

#include "Network/config.h"

#include "Network/ShipHandle/ShipHandle.h"

namespace cg {
    using RegisterShipCallback = std::function <kj::Own <ShipHandleImpl> (Spaceship const &, Backend::ShipHandle::Client)>;

    class ShipRegistrarImpl final: public Backend::ShipRegistrar::Server {
    private:
        RegisterShipCallback onRegisterShip;

        void log (std::string const & msg);

    public:
        ShipRegistrarImpl() = default;

        void setOnRegisterShip (RegisterShipCallback && callback);

        ::kj::Promise <void> registerShip (RegisterShipContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SHIPREGISTRAR_H

/* Copyright © 2022 Aaron Alef */
