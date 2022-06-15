#ifndef CAPSTONE_NETWORK_SHIPSINK_H
#define CAPSTONE_NETWORK_SHIPSINK_H

#include "Data/Item.h"

#include <functional>

namespace cg {
    using DoneCallback = std::function <kj::Promise <void> ()>;
    using SendItemCallback = std::function <kj::Promise <void> (Item const &)>;
    using GetShipPromiseCallback = std::function <kj::Promise <Spaceship> ()>;

    /// Implementation of the ShipSink::Server interface
    class ShipSinkImpl final: public Backend::ShipSink::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg) const;

        DoneCallback onDone;
        SendItemCallback onSendItem;
        GetShipPromiseCallback onGetShip;

    public:
        /// Tell the sink hwo to handle done requests
        inline void setOnDone (DoneCallback && callback) { onDone = std::move (callback); };
        /// Tell the sink hwo to handle sendItem requests
        inline void setOnSendItem (SendItemCallback && callback) { onSendItem = std::move (callback); };
        /// Tell the sink hwo to handle getShip requests
        inline void setOnGetShip (GetShipPromiseCallback && callback) { onGetShip = std::move (callback); };

        ::kj::Promise <void> done     (DoneContext     context) override;
        ::kj::Promise <void> sendItem (SendItemContext context) override;
        ::kj::Promise <void> getShip  (GetShipContext  context) override;
    };

} // cg

#endif //CAPSTONE_NETWORK_SHIPSINK_H

/* Copyright © 2022 Aaron Alef */
