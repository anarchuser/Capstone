#ifndef CAPSTONE_NETWORK_CONFIG_H
#define CAPSTONE_NETWORK_CONFIG_H

#include "Network/generated/synchro.capnp.h"

#include <kj/debug.h>
#include <kj/memory.h>
#include <kj/common.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

#include "helper.h"

#include <string>
#include <iostream>
#include <sstream>

namespace cg {
    typedef std::string ClientID;
    typedef std::string ShipName;

    using Backend_t    = ::Backend::Client;
    using Synchro_t    = ::Backend::Synchro::Client;
    using Registrar_t  = ::Backend::Registrar::Client;
    using ShipHandle_t = ::Backend::ShipHandle::Client;
    using ShipSink_t   = ::Backend::ShipSink::Client;
}

#endif //CAPSTONE_NETWORK_CONFIG_H

/* Copyright © 2022 Aaron Alef */
