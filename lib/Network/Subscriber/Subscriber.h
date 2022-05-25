#ifndef CAPSTONE_NETWORK_SUBSCRIBER_H
#define CAPSTONE_NETWORK_SUBSCRIBER_H

#include "Network/Backend.h"

namespace cg {

class SubscriberImpl final: public Backend::Subscriber::Server {
    private:
        /// Log function of this implementation
        void log (std::string const & msg);

    public:
    };

} // cg

#endif //CAPSTONE_NETWORK_SUBSCRIBER_H

/* Copyright © 2022 Aaron Alef */
