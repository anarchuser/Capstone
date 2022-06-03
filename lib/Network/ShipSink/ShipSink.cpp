#include "ShipSink.h"

namespace cg {
    void ShipSinkImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ShipSink @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str ());
#ifdef DEBUG_MINOR
        debug_stdout (ss.str());
#endif
    }

    ::kj::Promise <void> ShipSinkImpl::done (DoneContext context) {
        log ("Ship closed stream");

        try {
            onDone();
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipSink::done called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipSinkImpl::sendItem (SendItemContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasItem());
        KJ_REQUIRE (params.getItem().hasDirection());
        Direction directions (params.getItem().getDirection());

        KJ_REQUIRE (params.getItem().hasSpaceship());
        Spaceship spaceship (params.getItem().getSpaceship());

        try {
            onSendItem ({directions, spaceship});
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipSink::sendItem called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipSinkImpl::getShip (GetShipContext context) {
        auto results = context.getResults();

        try {
            onGetShip().initialise (results.initShip());
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipSink::getShip called without valid callback registered");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
