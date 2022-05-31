#include "Synchro.h"

namespace cg {
    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
#ifdef DEBUG_MINOR
        debug_stdout (ss.str());
#endif
    }

    ::kj::Promise<void> SynchroImpl::connect (ConnectContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        log ("Connection request received from client " + std::string (params.getId()));

        KJ_REQUIRE (params.hasSynchro());
        KJ_REQUIRE (params.hasRegistrar());
        auto results = context.getResults();

        try {
            results.setRegistrar (onConnect (params.getId(), params.getSynchro(), params.getRegistrar()));
            results.setId (ID);
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "Synchro::connect called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> SynchroImpl::share (ShareContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        KJ_REQUIRE (params.hasSynchro());

        try {
            onShare (params.getId(), params.getSynchro());
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "Synchro::share called without valid callback registered");
        }
    }
}

/* Copyright © 2022 Aaron Alef */
