#include "Registrar.h"

namespace cg {
    void RegistrarImpl::log (std::string const & msg) const {
        std::stringstream ss;
        ss << "Registrar @" << this << ": '" << msg << "'";
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
            auto sink = onRegisterShip (Spaceship (params.getShip()), params.getHandle());
            context.initResults().setSink (kj::mv (sink));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "RegistrarImpl::registerShip called without valid callback registered");
        } catch (std::exception & e) { /* Duplicate username, ignore */ }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
