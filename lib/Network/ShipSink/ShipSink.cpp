#include "ShipSink.h"

namespace cg {
    void ShipSinkImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ShipSink @" << this << ": '" << msg << "'";
                KJ_DLOG (INFO, ss.str ());
        std::cout << ss.str () << std::endl;
    }

    ::kj::Promise <void> ShipSinkImpl::done (DoneContext context) {
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
        auto item = params.getItem();
        KJ_REQUIRE (item.hasDirection());
        auto directions = item.getDirection();

        try {
            return onSendItem (Direction (directions));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipSink::sendItem called without valid callback registered");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
