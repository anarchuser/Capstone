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

    ::kj::Promise<void> ShipCallbackImpl::sendSink (SendSinkContext context) {
        auto spaceship = context.getParams().getSpaceship();
        auto position = spaceship.getPosition();
        auto velocity = spaceship.getVelocity();
        auto username = spaceship.getUsername();
        auto angle    = spaceship.getAngle();
        log ("New Spaceship: " + std::string (username));

        try {
            context.getResults().setSink (onSendSink ({
                username, {
                    position.getX(),
                    position.getY()
                }, {
                    velocity.getX(),
                    velocity.getY()
                }, angle
                }));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipCallback::sendSink called without valid callback registered");
        }

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
