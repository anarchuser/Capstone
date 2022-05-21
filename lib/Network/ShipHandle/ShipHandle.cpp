#include "ShipHandle.h"

namespace cg {
    void ShipHandleImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ShipHandle @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    void ShipHandleImpl::setOnGetSpaceship (GetSpaceshipHandle && onGetSpaceship) {
        this->onGetSpaceship = std::move (onGetSpaceship);
    }
    void ShipHandleImpl::setOnSetSpaceship (SetSpaceshipHandle && onSetSpaceship) {
        this->onSetSpaceship = std::move (onSetSpaceship);
    }

    ::kj::Promise<void> ShipHandleImpl::getSpaceship (GetSpaceshipContext context) {
        try {
            onGetSpaceship ()
                .initialise (context.getResults().getSpaceship());

        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::getSpaceship called without valid callback registered");
        }

        return kj::READY_NOW;
    }

    ::kj::Promise<void> ShipHandleImpl::setSpaceship (SetSpaceshipContext context) {
        Spaceship ship (context.getParams().getSpaceship());

        try {
            onSetSpaceship (ship);

        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::setSpaceship called without valid callback registered");
        }

        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipHandleImpl::destroy (DestroyContext context) {
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
