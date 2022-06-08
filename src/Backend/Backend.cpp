#include "Backend.h"

namespace kt {
    Backend::Backend (std::size_t seed, std::string const & address, unsigned short port)
            : seed {seed}
            , address {address + ':' + std::to_string (port)}
            , server_thread {CLOSURE (this, & Backend::serve)}
            {
                getPort();
            }
    
    Backend::~Backend () {
        if (!stop) logs::messageln ("Shutting down backend");
        stop = true;
        if (server_thread.joinable()) server_thread.join();
    }
    
    void Backend::serve () {
        for (int i = 0; i < 100; i++) {
            try {
                auto server = capnp::EzRpcServer (kj::heap <cg::BackendImpl> (seed, hostname()), address);
                port = server.getPort ().wait (server.getWaitScope ());
                logs::messageln ("Backend starts serving now on address '%s'", address.c_str());

                auto & exec = kj::getCurrentThreadExecutor();
                auto & waitscope = server.getWaitScope();
                while (!stop) {
                    exec.executeAsync ([this] () {
                        std::this_thread::yield ();
                    }).wait (waitscope);
                }
                break;
            } catch (std::exception & e) {
                std::string desc {e.what()};
                if (desc.find ("Eventloop destroyed") != std::string::npos) return;
                std::this_thread::sleep_for (std::chrono::milliseconds (500));
            }
        }
        logs::error ("Failed to set up server");
    }
    
    unsigned short Backend::getPort () const {
        while (port == -1) std::this_thread::yield();
        return port;
    }

    bool Backend::ping (std::string const & ip, unsigned short port) {
        auto client = capnp::EzRpcClient (ip, port);
        auto request = client.getMain <::Backend>().pingRequest();
        try {
            request.send().wait (client.getWaitScope());
            return true;
        } catch (std::exception & e) {
            std::string exception (e.what());
            if (exception.find ("disconnected: connect(): Connection refused") < 0) {
                throw e;
            }
            logs::warning ("Ping '%s:%d': Connection refused", ip.c_str(), port + 65536);
        }
        return false;
    }
}

/* Copyright (C) Aaron Alef */