#include "Backend.h"

namespace kt {
    Backend::Backend (std::size_t seed, std::string address)
            : seed {seed}
            , address {std::move (address)}
            , server_thread {& Backend::serve, this}
            {
                getPort();
            }
    
    Backend::~Backend () noexcept {
        if (!stop) logs::messageln ("Shutting down backend");
        stop = true;
        if (server_thread.joinable()) server_thread.join();
    }
    
    void Backend::serve () {
        for (int i = 0; i < 100; i++) {
            try {
                auto server = capnp::EzRpcServer (kj::heap <cg::BackendImpl> (seed, USERNAME), address);
                port = server.getPort ().wait (server.getWaitScope ());
                logs::messageln ("Backend starts serving now");

                auto & exec = kj::getCurrentThreadExecutor();
                while (!stop) {
                    exec.executeAsync ([this] () {
                        std::this_thread::yield ();
                    }).wait (server.getWaitScope ());
                }
                return;
            } catch (...) {
                std::this_thread::sleep_for (std::chrono::milliseconds (500));
            }
        }
        logs::error ("Failed to set up server");
    }
    
    unsigned short Backend::getPort () const {
        while (port == -1) std::this_thread::yield();
        return port;
    }
    std::string const & Backend::getAddress() const {
        return address;
    }

    bool Backend::ping (std::string const & ip, short port) {
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
        }
        return false;
    }
}

/* Copyright (C) Aaron Alef */