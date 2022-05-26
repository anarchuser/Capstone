#include "Synchro.h"

namespace cg {
    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro @" << this << ": '" << msg << "'";
                KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    ::kj::Promise<void> SynchroImpl::connect (ConnectContext context) {
        log ("Connection request received");
        auto params = context.getParams();
        KJ_REQUIRE (params.hasSynchro());
        KJ_REQUIRE (params.hasRegistrar());
        auto results = context.getResults();

        try {
            results.setRegistrar (onConnect (params.getSynchro(), params.getRegistrar()));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "Synchro::connect called without valid callback registered");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
