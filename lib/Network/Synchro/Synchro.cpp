#include "Synchro.h"

namespace cg {
    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "SynchroImpl @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    void SynchroImpl::setOnConnect (ConnectCallback && onConnect) {
        this->onConnect = onConnect;
    }

    kj::Promise <void> SynchroImpl::connect (ConnectContext context) {
        log ("Connection request received");

        auto params = context.getParams();
        auto results = context.initResults ();
        try {
            if (params.hasOther()) {
                results.setRegistrar (onConnect (params.getOther().connectRequest().send().getRegistrar()));
            }
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "Synchro::connect called without valid callback registered");
        }

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
