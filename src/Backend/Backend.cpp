#include "Backend.h"

namespace kt {
    Backend::Backend (std::size_t seed, std::string address)
            : seed {seed}
            , address {std::move (address)}
            , server_thread {& Backend::serve, this} {
        getPort();
        logs::messageln ("Set up backend at '%s'", this->address.c_str());
    }
    
    Backend::~Backend () noexcept {
        stop = true;
        server_thread.join();
    }
    
    void Backend::serve () {
        auto impl = kj::heap <cg::SynchroImpl> (seed);
        while (true) {
            try {
                auto server = capnp::EzRpcServer (kj::mv (impl), address);
                port = server.getPort().wait (server.getWaitScope());
                break;
            } catch (std::exception & e) {
                logs::warning ("Failed to bind address to '%s'. Retrying...", address.c_str());
                std::this_thread::yield();
            }
        }
        while (!stop) std::this_thread::yield();
    }
    
    unsigned short Backend::getPort () const {
        while (port == -1) std::this_thread::yield();
        return port;
    }
    std::string const & Backend::getAddress() const {
        return address;
    }

    void Backend::connect (std::string remote, short port) {
    }

    bool Backend::ping (std::string const & ip, short port) {
        auto client = capnp::EzRpcClient (ip, port);
        auto request = client.getMain <Synchro>().pingRequest();
        try {
            request.send().wait (client.getWaitScope());
            return true;
        } catch (std::exception & e) {
            logs::warning ("Backend::ping(): Connection refused:\n'%s'", e.what());
        }
    }
}

/* Copyright (C) Aaron Alef */