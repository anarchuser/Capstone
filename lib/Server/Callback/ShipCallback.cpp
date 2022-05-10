#include "ShipCallback.h"

namespace cg {
    void ShipCallbackImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Callback @" << this << ": '" << msg << "'" << std::endl;
        KJ_DLOG (INFO, ss.str());
    }

    void ShipCallbackImpl::setOnSendSink (SendSinkCallbackHandle const & onSendSink) {
        this->onSendSink = onSendSink;
    }

    ::kj::Promise<void> ShipCallbackImpl::sendSink (SendSinkContext context) {
        // TODO: Tie this to ctor of SpaceShip
        auto params = context.getParams();
        log ("New Spaceship: " + std::string (params.getUsername()));

        onSendSink (params.getUsername());

//        auto ptr = onSendSink;
//        if (auto callback = ptr.lock()) {
//            (* callback) (params.getUsername ());
//        } else KJ_DLOG (WARNING, "onSendSink called whilst callback expired");
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
