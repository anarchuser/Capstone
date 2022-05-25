#include "Registrar.h"

namespace cg {
    void RegistrarImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Registrar @" << this << ": '" << msg << "'";
                KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }
}

/* Copyright © 2022 Aaron Alef */
