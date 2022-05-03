#ifndef CAPSTONE_DIRECTIONCALLBACKIMPL_H
#define CAPSTONE_DIRECTIONCALLBACKIMPL_H

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

    class DirectionCallbackImpl final: public Synchro::DirectionCallback::Server {
    private:
        void log (const std::string& msg);

        std::function <void (Direction)> onSendDirection;

    public:
        DirectionCallbackImpl (std::function <void (Direction)> && onSendDirection);

        ::kj::Promise <void> sendDirection (SendDirectionContext context) override;
        // TODO: call this when spaceship was destroyed
        ::kj::Promise <void> done (DoneContext context) override;
    };

} // cg

#endif //CAPSTONE_DIRECTIONCALLBACKIMPL_H

/* Copyright © 2022 Aaron Alef */
