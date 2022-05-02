#ifndef _CAPSTONE_BACKEND_H
#define _CAPSTONE_BACKEND_H

#include "config.h"

#include "oxygine-framework.h"

#include "helper.h"

#include "Connection.h"
#include "Server/generated/synchro.capnp.h"
#include "Server/Server.h"
#include <capnp/ez-rpc.h>
#include <kj/debug.h>
#include <thread>
#include <atomic>
#include <memory>

#include <unordered_map>
#include <string>

namespace kt {
    using namespace oxygine;

    class Backend {
    private:
        kj::Own <capnp::EzRpcServer> server;
        std::thread server_thread;
        std::string const address;
        std::atomic <int> port = -1;

        std::vector <std::unique_ptr <Connection>> connections;
        
        void serve ();
        
    public:
        explicit Backend (std::string address);
        ~Backend () noexcept;

        [[nodiscard]] unsigned short getPort() const;
        [[nodiscard]] std::string const & getAddress() const;

        void connect (Direction const * direction, std::string address, unsigned short port);
        void update();
    };

} // kt

#endif //_CAPSTONE_BACKEND_H

/* Copyright (C) Aaron Alef */