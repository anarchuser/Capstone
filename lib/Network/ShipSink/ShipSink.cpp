#include "ShipSink.h"

namespace cg {
    void ShipSinkImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ShipSink @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str ());
        debug_stdout (ss.str());
    }

    ::kj::Promise <void> ShipSinkImpl::done (DoneContext context) {
        log ("Ship closed stream");

        try {
            return onDone();
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipSink::done called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipSinkImpl::sendItem (SendItemContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasItem());
        KJ_REQUIRE (params.getItem().hasDirection());
        auto directions = params.getItem().getDirection();

        try {
            return onSendItem (Direction (directions));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipSink::sendItem called without valid callback registered");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
