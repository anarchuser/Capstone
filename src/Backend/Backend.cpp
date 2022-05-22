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
        if (auto & ship = KeyboardSpaceship::instance) ship->destroy();
        logs::messageln ("Shutting down backend");
        stop = true;
        server_thread.join();
    }
    
    void Backend::serve () {
        try {
            auto server = capnp::EzRpcServer (kj::heap <cg::BackendImpl> (seed), address);
            port = server.getPort().wait (server.getWaitScope());

            auto & exec = kj::getCurrentThreadExecutor();

            logs::messageln ("Backend starts serving now");

            while (! stop) {
                exec.executeAsync ([this] () {
                    std::this_thread::yield ();
                }).wait (server.getWaitScope ());
            }
            return;
        } catch (...) {
            logs::warning ("Failed to bind address to '%s'. Retrying...", address.c_str());
        }
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
            logs::warning ("Backend::ping(): Connection refused'");
        }
        return false;
    }
}

/* Copyright (C) Aaron Alef */