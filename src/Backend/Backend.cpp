#include "Backend.h"

namespace kt {
    Backend::Backend (std::string address)
            : address {address}
            , server_thread {& Backend::serve, this} {
        while (!server);
    }
    
    Backend::~Backend () noexcept {
        server->getWaitScope().cancelAllDetached();
        if (server_thread.joinable()) server_thread.join();
    }
    
    void Backend::serve () {
        server = kj::heap <capnp::EzRpcServer> (kj::heap <cg::SynchroImpl>(), address);
        port = server->getPort().wait (server->getWaitScope());
        logs::messageln ("Set up backend at '%s:%d'", address.c_str(), (int) port);
        kj::NEVER_DONE.wait (server->getWaitScope());
    }
    
    unsigned short Backend::getPort () const {
        while (port == -1);
        return port;
    }
}

/* Copyright (C) Aaron Alef */