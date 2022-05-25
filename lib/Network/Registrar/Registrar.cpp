#include "Registrar.h"

namespace cg {
    void RegistrarImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Registrar @" << this << ": '" << msg << "'";
                KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    void RegistrarImpl::setOnRegisterShip (RegisterShipCallback && callback) {
        onRegisterShip = callback;
    }

    ::kj::Promise <void> RegistrarImpl::registerShip (RegisterShipContext context) {
        log ("Register ship requested");

        try {
            onRegisterShip();
        } catch (std::bad_function_call & e) {
            KJ_DLOG (WARNING, "RegistrarImpl::registerShip called without valid callback registered");
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
