#ifndef CAPSTONE_DIRECTIONCALLBACK_H
#define CAPSTONE_DIRECTIONCALLBACK_H

#include "config.h"

#include "Server/generated/synchro.capnp.h"

#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include <iostream>
#include <functional>

#include "Direction/Direction.h"

namespace cg {
    struct DirectionCallback final {
        std::function <void (Direction)> const onSendDirection;
        std::function <void ()> const onDone;
    };

    class DirectionCallbackImpl final: public Synchro::DirectionCallback::Server {
    private:
        void log (const std::string& msg);

        DirectionCallback callback;

    public:
        explicit DirectionCallbackImpl (DirectionCallback && callback);

        ::kj::Promise <void> sendDirection (SendDirectionContext context) override;
        // TODO: call this when spaceship was destroyed
        ::kj::Promise <void> done (DoneContext context) override;
    };

} // cg

#endif //CAPSTONE_DIRECTIONCALLBACK_H

/* Copyright © 2022 Aaron Alef */
