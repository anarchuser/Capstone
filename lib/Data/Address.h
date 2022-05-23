#ifndef CAPSTONE_NETWORK_ADDRESS_H
#define CAPSTONE_NETWORK_ADDRESS_H

#include "Network/config.h"
#include <string>

namespace cg {
    struct Address {
        std::string const ip;
        unsigned short const port;

        Address(std::string && ip, unsigned short port);
        explicit Address (Backend::Address::Reader reader);

        void initialise (Backend::Address::Builder builder) const;

        explicit operator std::string () const;
    };
}

#endif //CAPSTONE_NETWORK_ADDRESS_H

/* Copyright © 2022 Aaron Alef */
