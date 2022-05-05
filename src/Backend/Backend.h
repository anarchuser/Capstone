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

#define ITOS(I) std::to_string (I)
#define IP(first, second, third, fourth) (ITOS (first) +'.'+ ITOS (second) +'.'+ ITOS (third) +'.'+ ITOS (fourth))

namespace kt {
    using namespace oxygine;

    class Backend {
    private:
        std::function <cg::DirectionCallback ()> onStreamDirections;
        kj::Own <capnp::EzRpcServer> server;
        std::thread server_thread;
        std::string const address;
        std::atomic <int> port = -1;

        kj::Executor const * thread_executor;

        std::unordered_map <std::string, std::unique_ptr <Connection>> connections;
        
        void serve ();

    public:
        Backend (std::string address, std::function <cg::DirectionCallback ()> && onStreamDirections);
        ~Backend () noexcept;

        [[nodiscard]] unsigned short getPort() const;
        [[nodiscard]] std::string const & getAddress() const;

        void connect (Direction const * direction, std::string remote, unsigned short port);
        void disconnectAll ();
        void update();
    };

} // kt

#endif //_CAPSTONE_BACKEND_H

/* Copyright (C) Aaron Alef */