#include "Synchro.h"

namespace cg {
    SynchroImpl::SynchroImpl (std::string id): ID {id} {}

    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        debug_stdout (ss.str());
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
}

/* Copyright © 2022 Aaron Alef */
