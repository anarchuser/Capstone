#include "Synchro.h"

namespace cg {
    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro @" << this << ": '" << msg << "'";
                KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }
}

/* Copyright © 2022 Aaron Alef */
