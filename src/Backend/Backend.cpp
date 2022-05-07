#include "Backend.h"

namespace kt {
    Backend::Backend (std::size_t seed, std::string address, std::function <cg::DirectionCallback ()> && onStreamDirections)
            : onStreamDirections {std::move (onStreamDirections)}
            , seed {seed}
            , address {std::move (address)}
            , server_thread {& Backend::serve, this} {
        while (!server);
    }
    
    Backend::~Backend () noexcept {
        thread_executor->executeSync ([this]() {
            server = nullptr;
        });
        if (server_thread.joinable()) server_thread.join();
//        server_thread.detach();
    }
    
    void Backend::serve () {
        auto impl = kj::heap <cg::SynchroImpl> (
                seed,
                std::function <cg::DirectionCallback ()> (onStreamDirections)
        );
        server = kj::heap <capnp::EzRpcServer> (kj::mv (impl), address);
        port = server->getPort().wait (server->getWaitScope());
        logs::messageln ("Set up backend at '%s'", address.c_str());
        thread_executor = & kj::getCurrentThreadExecutor();

        try {
            kj::NEVER_DONE.wait (server->getWaitScope());
        } catch (...) {
            logs::messageln ("Server stopped");
        }
    }
    
    unsigned short Backend::getPort () const {
        while (port == -1);
        return port;
    }
    std::string const & Backend::getAddress() const {
        return address;
    }

    void Backend::connect (Direction const * direction, std::string remote, unsigned short port) {
        connections.insert_or_assign (remote + ':' + std::to_string (port),
                                      std::make_unique <Connection> (onStreamDirections, seed, direction, remote, port));
    }

    void Backend::update () {
        for (auto & connection : connections) {
            connection.second->update();
        }
    }

    void Backend::disconnectAll () {
        connections.clear();
    }
}

/* Copyright (C) Aaron Alef */