#include "Synchro.h"

namespace cg {
    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "SynchroImpl @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    kj::Promise <void> SynchroImpl::disconnect (DisconnectContext context) {
        KJ_DLOG (WARNING, "disconnect: not yet implemented");
        KJ_UNIMPLEMENTED();

        return kj::READY_NOW;
    }

    kj::Promise <void> SynchroImpl::sendShip (SendShipContext context) {
        KJ_DLOG (WARNING, "sendShip: not yet implemented");
        KJ_UNIMPLEMENTED();

        return kj::READY_NOW;
    }

}

/* Copyright © 2022 Aaron Alef */
