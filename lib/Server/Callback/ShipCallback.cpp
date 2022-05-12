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
        auto username = params.getUsername();
        log ("New Spaceship: " + std::string (username));

        try {
            context.getResults().setShip (onSendSink (username));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipCallback::sendSink called without valid callback registered");
        }

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
