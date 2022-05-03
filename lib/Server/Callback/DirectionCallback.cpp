#include "DirectionCallback.h"

namespace cg {
    DirectionCallbackImpl::DirectionCallbackImpl (std::function<void (Direction)> && onSendDirection)
            : onSendDirection {std::move (onSendDirection)} {}

    void DirectionCallbackImpl::log (std::string const & msg) {
        LOG (INFO) << "DirectionCallback @" << this << ": '" << msg << "'";
        std::cout << "DirectionCallback @" << this << ": '" << msg << "'" << std::endl;
    }

    ::kj::Promise<void>
    DirectionCallbackImpl::sendDirection (SendDirectionContext context) {
        auto direction = context.getParams().getDirection();
        try {
            onSendDirection ({
                                     direction.getAccelerate (),
                                     direction.getDecelerate (),
                                     direction.getRotateLeft (),
                                     direction.getRotateRight ()
                             });
        } catch (std::bad_function_call &) {
            LOG (WARNING) << "Received updateDirection before callback has been initialised";
        }
        return kj::READY_NOW;
    }

    ::kj::Promise<void> DirectionCallbackImpl::done (DoneContext context) {
        log ("Client disconnected");
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
