#include "ShipRegistrar.h"

namespace cg {
    void ShipRegistrarImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "ShipRegistrar @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    void ShipRegistrarImpl::setOnRegisterShip (registerShipCallback && callback) {
        onRegisterShip = callback;
    }

    ::kj::Promise <void> ShipRegistrarImpl::registerShip (RegisterShipContext context) {
        auto params = context.getParams();
        auto results = context.initResults();

        try {
            results.setSink (onRegisterShip (Spaceship (params.getSpaceship()), params.getHandle()));
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "ShipRegistrar::registerShip called without valid callback registered");
        }

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
