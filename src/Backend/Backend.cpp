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
        auto server = [this] () {
            for (int i = 0; i < 100; i++) {
                try {
                    return capnp::EzRpcServer (kj::heap <cg::BackendImpl> (seed, hostname ()), address);
                } catch (std::exception & e) {
                    // Server could not be started, e.g., because the port is still blocked. Wait, then retry
                    std::this_thread::sleep_for (std::chrono::milliseconds (500));
                }
            }
        }();
        port = server.getPort ().wait (server.getWaitScope ());
        logs::messageln ("Backend starts serving now on address '%s'", address.c_str ());

        auto & exec = kj::getCurrentThreadExecutor ();
        kj::WaitScope & waitscope = server.getWaitScope ();

        // NEVER_DONE.poll executes events until the queue is empty. If it is, check if stop has been requested.
        kj::Promise<void> NEVER_DONE = kj::NEVER_DONE;
        while (!stop) NEVER_DONE.poll (waitscope);
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