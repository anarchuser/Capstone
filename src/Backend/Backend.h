#ifndef _CAPSTONE_BACKEND_H
#define _CAPSTONE_BACKEND_H

#include "config.h"

#include "oxygine-framework.h"

#include "helper.h"

#include "Server/generated/synchro.capnp.h"
#include "Server/Server.h"
#include <capnp/ez-rpc.h>
#include <kj/debug.h>
#include <thread>
#include <string>
#include <atomic>

namespace kt {
    using namespace oxygine;

    class Backend {
    private:
        kj::Own <capnp::EzRpcServer> server;
        std::thread server_thread;
        std::string const address;
        std::atomic <int> port = -1;
        
        void serve ();
        
    public:
        [[nodiscard]] unsigned short getPort() const;
        
        explicit Backend (std::string address);
        ~Backend () noexcept;
    };

} // kt

#endif //_CAPSTONE_BACKEND_H

/* Copyright (C) Aaron Alef */