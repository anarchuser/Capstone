#include "ShipCallback.h"

namespace cg {
    void ShipCallbackImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Callback @" << this << ": '" << msg << "'" << std::endl;
        KJ_LOG (INFO, ss.str());
    }

    ::kj::Promise<void> ShipCallbackImpl::sendSink (SendSinkContext context) {
        // TODO: Tie this to ctor of RemoteSpaceship
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
