#include "Connection.h"

namespace kt {
    Connection::Connection (cg::JoinCallback & onJoin, std::size_t seed, Direction const * direction, std::string address, unsigned short port)
            : seed {seed}
            , direction {direction}
            , rpcClient {kj::heap <capnp::EzRpcClient> (address, port)}
            , client {rpcClient->getMain<Synchro>()}
            , waitscope {rpcClient->getWaitScope()}
            {
                initCallback (onJoin);
            }

    Connection::~Connection () noexcept {
        // TODO: Invalidate callback pointer given to client
    }

    void Connection::initCallback (cg::JoinCallback & onJoin) {
        auto request = client.joinRequest();
        auto server = kj::heap <cg::SynchroImpl> (seed);
        auto callback = kj::heap <cg::CallbackDirectionImpl> ();

        request.initOther().setValue (kj::mv (server));
        request.setCallback (kj::mv (callback));
        request.send().wait (waitscope);
    }

    bool Connection::ping (std::string const & ip, short port) {
        auto client = capnp::EzRpcClient (ip, port);
        try {
            client.getMain <Synchro> ().pingRequest().send().wait (client.getWaitScope());
            return true;
        } catch (...) {
            logs::warning ("Failed to ping to '%s:%d'", ip.c_str(), port);
        }
        return false;
    }
}

/* Copyright © 2022 Aaron Alef */
