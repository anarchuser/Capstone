#include "ShipCallback.h"

namespace cg {
    void ShipCallbackImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ShipCallback @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    void ShipCallbackImpl::setOnSendSink (SendSinkCallbackHandle const & onSendSink) {
        this->onSendSink = onSendSink;
    }
    void ShipCallbackImpl::setOnGetSpaceship (GetSpaceshipCallbackHandle const & onGetSpaceship) {
        this->onGetSpaceship = onGetSpaceship;
    }
    void ShipCallbackImpl::setOnSetSpaceship (SetSpaceshipCallbackHandle const & onSetSpaceship) {
        this->onSetSpaceship = onSetSpaceship;
    }

    ::kj::Promise<void> ShipCallbackImpl::sendSink (SendSinkContext context) {
        auto spaceship = context.getParams().getSpaceship();
        auto username = spaceship.getUsername();
        log ("New Spaceship: " + std::string (username));

        try {
            context.getResults().setSink (onSendSink (Spaceship (spaceship)));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipCallback::sendSink called without valid callback registered");
        }

        return kj::READY_NOW;
    }

    ::kj::Promise<void> ShipCallbackImpl::getSpaceship (GetSpaceshipContext context) {
        try {
            onGetSpaceship ()
                .initialise (context.getResults().getSpaceship());

        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipCallback::getSpaceship called without valid callback registered");
        }

        return kj::READY_NOW;
    }

    ::kj::Promise<void> ShipCallbackImpl::setSpaceship (SetSpaceshipContext context) {
        auto ship = context.getParams().getSpaceship();

        try {
            onSetSpaceship (Spaceship (ship));

        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipCallback::setSpaceship called without valid callback registered");
        }

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
