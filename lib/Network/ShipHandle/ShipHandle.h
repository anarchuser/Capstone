#ifndef CAPSTONE_NETWORK_SHIPCALLBACK_H
#define CAPSTONE_NETWORK_SHIPCALLBACK_H

#include "Network/config.h"

#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include <atomic>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>

#include "Data/Direction.h"
#include "Data/Spaceship.h"

namespace cg {
    using DoneCallback         = std::function <void ()>;
    using SendItemCallback     = std::function <void (Direction)>;
    using GetSpaceshipCallback = std::function <Spaceship ()>;
    using SetSpaceshipCallback = std::function <void (Spaceship)>;

    class ShipHandleImpl final: public Backend::ShipHandle::Server {
    private:
        DoneCallback         onDone;
        SendItemCallback     onSendItem;
        GetSpaceshipCallback onGetSpaceship;
        SetSpaceshipCallback onSetSpaceship;

        void log (std::string const & msg);

    public:
        ShipHandleImpl() = default;
        ~ShipHandleImpl() = default;

        void setOnDone (DoneCallback && onDone);
        void setOnSendItem (SendItemCallback && onSendItem);
        void setOnGetSpaceship (GetSpaceshipCallback && onGetSpaceship);
        void setOnSetSpaceship (SetSpaceshipCallback && onSetSpaceship);

        ::kj::Promise <void> done (DoneContext context) override;
        ::kj::Promise <void> sendItem (SendItemContext context) override;
        ::kj::Promise <void> getSpaceship (GetSpaceshipContext context) override;
        ::kj::Promise <void> setSpaceship (SetSpaceshipContext context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_SHIPCALLBACK_H

/* Copyright © 2022 Aaron Alef */
