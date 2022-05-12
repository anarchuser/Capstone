#include "ShipCallback.h"

namespace cg {
    void ShipCallbackImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ShipCallback @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    void ShipCallbackImpl::setOnSendSink (SendSinkCallbackHandle const & onSendSink) {
        this->onSendSink = onSendSink;
    }

    ::kj::Promise<void> ShipCallbackImpl::sendSink (SendSinkContext context) {
        auto params = context.getParams();
        log ("New Spaceship: " + std::string (params.getUsername()));

        try {
            context.getResults().setShip (onSendSink (params.getUsername()));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipCallback::sendSink called without valid callback registered");
        }

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
