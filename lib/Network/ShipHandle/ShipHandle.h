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
#include "Network/ItemSink/ItemSink.h"

namespace cg {
    using GetSpaceshipHandle       = std::function <Spaceship ()>;
    using SetSpaceshipHandle       = std::function <void (Spaceship)>;

    class ShipHandleImpl final: public Backend::ShipHandle::Server {
    private:
        GetSpaceshipHandle onGetSpaceship;
        SetSpaceshipHandle onSetSpaceship;

        void log (std::string const & msg);

    public:
        ShipHandleImpl() = default;
        ~ShipHandleImpl() = default;

        void setOnGetSpaceship (GetSpaceshipHandle && onGetSpaceship);
        void setOnSetSpaceship (SetSpaceshipHandle && onSetSpaceship);

        ::kj::Promise <void> getSpaceship (GetSpaceshipContext context) override;
        ::kj::Promise <void> setSpaceship (SetSpaceshipContext context) override;
        ::kj::Promise <void> destroy (DestroyContext context) override;
    };
} // cg

#endif //CAPSTONE_NETWORK_SHIPCALLBACK_H

/* Copyright © 2022 Aaron Alef */
