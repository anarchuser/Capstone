#include "Callback.h"

namespace cg {
    void DirectionCallbackImpl::log (std::string const & msg) {
        // TODO: allow setting an external logging function
        std::cout << "DirectionCallback @" << this << ": '" << msg << "'" << std::endl;
    }

    ::kj::Promise<void>
    DirectionCallbackImpl::sendDirection (SendDirectionContext context) {
        auto direction = context.getParams().getDirection();
        callback.onSendDirection ({
            direction.getAccelerate (),
            direction.getDecelerate (),
            direction.getRotateLeft (),
            direction.getRotateRight ()});
        return kj::READY_NOW;
    }

    ::kj::Promise<void> DirectionCallbackImpl::done (DoneContext context) {
        log ("Client disconnected");
        callback.onDone();
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
