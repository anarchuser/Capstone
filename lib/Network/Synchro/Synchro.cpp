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
    void SynchroImpl::setOnSync (SyncCallback && onSync) {
        this->onSync = onSync;
    }

    kj::Promise <void> SynchroImpl::connect (ConnectContext context) {
        log ("Connection request received");

        auto params = context.getParams();

        try {
            onConnect();
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "Synchro::connect called without valid callback registered");
        }

        if (params.hasOther()) {
            return params.getOther().connectRequest ().send().ignoreResult();
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> SynchroImpl::sync (SyncContext context) {
        log ("Syncronisation request received");

        auto params = context.getParams();
        KJ_REQUIRE (params.hasRegistrate())

        try {
            onSync (params.getRegistrate());
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "Synchro::sync called without valid callback registered");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
