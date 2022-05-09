#ifndef CAPSTONE_SHIPCALLBACK_H
#define CAPSTONE_SHIPCALLBACK_H

#include "config.h"

#include "Server/generated/synchro.capnp.h"

#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>

#include "Direction/Direction.h"

namespace cg {
    class ShipCallbackImpl final: public Synchro::ShipCallback::Server {
    private:
        void log (std::string const & msg);

    public:
        ShipCallbackImpl() = default;
        ~ShipCallbackImpl() = default;

        ::kj::Promise<void> sendSink (SendSinkContext context) override;
    };
} // cg

#endif //CAPSTONE_SHIPCALLBACK_H

/* Copyright © 2022 Aaron Alef */
