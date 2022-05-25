#include "Subscriber.h"

namespace cg {
    void SubscriberImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Subscriber @" << this << ": '" << msg << "'";
                KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }
}

/* Copyright © 2022 Aaron Alef */
