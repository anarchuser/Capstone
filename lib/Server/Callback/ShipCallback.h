#ifndef CAPSTONE_SHIPCALLBACK_H
#define CAPSTONE_SHIPCALLBACK_H

#include "config.h"

#include "Server/generated/synchro.capnp.h"
#include "ItemSink.h"

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
    using SendSinkCallback       = std::function <kj::Own <cg::ItemSinkImpl> (Spaceship)>;
    using SendSinkCallbackHandle = SendSinkCallback;
    using GetSpaceshipCallback       = std::function <Spaceship ()>;
    using GetSpaceshipCallbackHandle = GetSpaceshipCallback;
    using SetSpaceshipCallback       = std::function <void (Spaceship)>;
    using SetSpaceshipCallbackHandle = SetSpaceshipCallback;

    class ShipCallbackImpl final: public Synchro::ShipCallback::Server {
    private:
        SendSinkCallback onSendSink;
        GetSpaceshipCallback onGetSpaceship;
        SetSpaceshipCallback onSetSpaceship;

        void log (std::string const & msg);

    public:
        ShipCallbackImpl() = default;
        ~ShipCallbackImpl() = default;

        void setOnSendSink (SendSinkCallbackHandle const & onSendSink);
        void setOnGetSpaceship (GetSpaceshipCallbackHandle const & onGetSpaceship);
        void setOnSetSpaceship (SetSpaceshipCallbackHandle const & onSetSpaceship);

        ::kj::Promise<void> sendSink (SendSinkContext context) override;
        ::kj::Promise<void> getSpaceship (GetSpaceshipContext context) override;
        ::kj::Promise<void> setSpaceship (SetSpaceshipContext context) override;
    };
} // cg

#endif //CAPSTONE_SHIPCALLBACK_H

/* Copyright © 2022 Aaron Alef */
