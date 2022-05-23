#include "ShipHandle.h"

namespace cg {
    void ShipHandleImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ShipHandle @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    void ShipHandleImpl::setOnDone (DoneCallback && onDone) {
        this->onDone = onDone;
    }
    void ShipHandleImpl::setOnSendItem (SendItemCallback && onSendItem) {
        this->onSendItem = onSendItem;
    }
    void ShipHandleImpl::setOnGetSpaceship (GetSpaceshipCallback && onGetSpaceship) {
        this->onGetSpaceship = onGetSpaceship;
    }
    void ShipHandleImpl::setOnSetSpaceship (SetSpaceshipCallback && onSetSpaceship) {
        this->onSetSpaceship = onSetSpaceship;
    }

    ::kj::Promise <void> ShipHandleImpl::done (DoneContext context) {
        try {
            onDone();
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::done called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipHandleImpl::sendItem (SendItemContext context) {
        auto directions = context.getParams().getItem().getDirection();
        try {
            onSendItem (Direction (directions));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::sendItem called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipHandleImpl::getSpaceship (GetSpaceshipContext context) {
        try {
            onGetSpaceship().initialise (context.getResults().getSpaceship());
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::getSpaceship called without valid callback registered");
        }
        return kj::READY_NOW;
    }

    ::kj::Promise <void> ShipHandleImpl::setSpaceship (SetSpaceshipContext context) {
        Spaceship ship (context.getParams().getSpaceship());
        try {
            onSetSpaceship (ship);
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipHandle::setSpaceship called without valid callback registered");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
