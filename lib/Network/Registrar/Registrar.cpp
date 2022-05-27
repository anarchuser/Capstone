#include "Registrar.h"

namespace cg {
    void RegistrarImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Registrar @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        debug_stdout (ss.str());
    }

    ::kj::Promise <void> RegistrarImpl::registerShip (RegisterShipContext context) {
//        log ("Register ship requested");
        auto params = context.getParams();
        KJ_REQUIRE (params.hasShip());
        KJ_REQUIRE (params.hasHandle());

        try {
            context.getResults().setSink (onRegisterShip (Spaceship (params.getShip()), params.getHandle()));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "RegistrarImpl::registerShip called without valid callback registered");
        }
        catch (std::exception & e) {
//            log ("Unknown exception caught; likely Duplicate username");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
