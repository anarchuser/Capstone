#ifndef CAPSTONE_NETWORK_SHIPSINK_H
#define CAPSTONE_NETWORK_SHIPSINK_H

#include "Data/Item.h"

#include <functional>

namespace cg {
    using DoneCallback = std::function <void ()>;
    using SendItemCallback = std::function <void (Item const &)>;
    using GetShipPromiseCallback = std::function <kj::Promise <Spaceship> ()>;

    class ShipSinkImpl final: public Backend::ShipSink::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

        DoneCallback onDone;
        SendItemCallback onSendItem;
        GetShipPromiseCallback onGetShip;

    public:
        inline void setOnDone (DoneCallback && callback) { onDone = callback; };
        inline void setOnSendItem (SendItemCallback && callback) { onSendItem = callback; };
        inline void setOnGetShip (GetShipPromiseCallback && callback) { onGetShip = callback; };

        ::kj::Promise <void> done (DoneContext context) override;
        ::kj::Promise <void> sendItem (SendItemContext context) override;
        ::kj::Promise <void> getShip (GetShipContext context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SHIPSINK_H

/* Copyright © 2022 Aaron Alef */
