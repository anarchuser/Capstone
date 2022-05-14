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
#include <memory>
#include <thread>

#include "Data/Direction.h"

namespace cg {
    using SendSinkCallback       = std::function <kj::Own <cg::ItemSinkImpl> (std::string const &)>;
    using SendSinkCallbackHandle = std::function <kj::Own <cg::ItemSinkImpl> ()>;

    class ShipCallbackImpl final: public Synchro::ShipCallback::Server {
    private:
        SendSinkCallback onSendSink;

        void log (std::string const & msg);

    public:
        ShipCallbackImpl() = default;
        ~ShipCallbackImpl() = default;

        void setOnSendSink (SendSinkCallbackHandle const & onSendSink);

        ::kj::Promise<void> sendSink (SendSinkContext context) override;
    };
} // cg

#endif //CAPSTONE_SHIPCALLBACK_H

/* Copyright © 2022 Aaron Alef */
