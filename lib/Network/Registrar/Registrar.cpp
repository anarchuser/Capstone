#include "Registrar.h"

namespace cg {
    RegistrarImpl::RegistrarImpl (ClientID id)
            : ID {std::move (id)}
            {}

    void RegistrarImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Registrar {" << ID << "} @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
#ifdef DEBUG_MINOR
        debug_stdout (ss.str());
#endif
    }

    ::kj::Promise <void> RegistrarImpl::registerShip (RegisterShipContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasShip());
        KJ_REQUIRE (params.hasHandle());
        log ("Register ship requested: " + std::string (params.getShip().getUsername()));

        try {
            context.getResults().setSink (onRegisterShip (Spaceship (params.getShip()), ID, params.getHandle()));
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
