#ifndef CAPSTONE_NETWORK_SHIPHANDLE_H
#define CAPSTONE_NETWORK_SHIPHANDLE_H

#include "Network/Backend.h"
#include "Network/ShipSink/ShipSink.h"

#include <functional>

namespace cg {
    using GetSinkCallback = std::function <kj::Own <ShipSinkImpl> ()>;
    using GetShipCallback = std::function <Spaceship ()>;
    using SetShipCallback = std::function <void (Spaceship)>;

    class ShipHandleImpl final: public Backend::ShipHandle::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

        GetSinkCallback onGetSink;
        GetShipCallback onGetShip;
        SetShipCallback onSetShip;

    public:
        inline void setOnGetSink (GetSinkCallback && callback) { onGetSink = callback; };
        inline void setOnGetShip (GetShipCallback && callback) { onGetShip = callback; };
        inline void setOnSetShip (SetShipCallback && callback) { onSetShip = callback; };

        ::kj::Promise <void> getSink (GetSinkContext context) override;
        ::kj::Promise <void> getShip (GetShipContext context) override;
        ::kj::Promise <void> setShip (SetShipContext context) override;
        ::kj::Promise <void> ping (PingContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SHIPHANDLE_H

/* Copyright © 2022 Aaron Alef */
