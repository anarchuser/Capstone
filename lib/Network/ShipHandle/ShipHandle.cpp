#include "ShipHandle.h"

namespace cg {
    void ShipHandleImpl::log (std::string const & msg) const {
        std::stringstream ss;
        ss << "ShipHandle @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str ());
#ifdef DEBUG_MINOR
        debug_stdout (ss.str());
#endif
    }

    ::kj::Promise <void> ShipHandleImpl::ping (PingContext context) {
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipHandleImpl::getSink (GetSinkContext context) {
        log ("Sink requested");
        auto results = context.getResults();

        try {
            results.setSink (onGetSink());
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::getSink called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipHandleImpl::getShip (GetShipContext context) {
        auto results = context.getResults();
        try {
            auto spaceship = onGetShip();
            log ("Data requested for ship " + spaceship.username);
            spaceship.initialise (results.initShip());
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::getShip called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipHandleImpl::setShip (SetShipContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasShip());
        Spaceship spaceship (params.getShip());
        log ("Change requested to ship " + spaceship.username);

        try {
            onSetShip (spaceship);
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::setShip called without valid callback registered");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
