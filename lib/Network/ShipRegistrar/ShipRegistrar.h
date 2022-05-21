#ifndef CAPSTONE_NETWORK_SHIPREGISTRAR_H
#define CAPSTONE_NETWORK_SHIPREGISTRAR_H

#include "Network/config.h"

#include "Network/ItemSink/ItemSink.h"
#include "Network/ShipHandle/ShipHandle.h"

namespace cg {
    using registerShipCallback = std::function <kj::Own <ItemSinkImpl> (Spaceship const &, Backend::ShipHandle::Client)>;

    class ShipRegistrarImpl final: public Backend::ShipRegistrar::Server {
    private:
        registerShipCallback onRegisterShip;

        void log (std::string const & msg);

    public:
        ShipRegistrarImpl() = default;

        void setOnRegisterShip (registerShipCallback && callback);

        ::kj::Promise <void> registerShip (RegisterShipContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SHIPREGISTRAR_H

/* Copyright © 2022 Aaron Alef */
