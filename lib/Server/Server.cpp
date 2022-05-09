#include "Server.h"

namespace cg {
    SynchroImpl::SynchroImpl (std::size_t seed, std::function <cg::DirectionCallback ()> && onStreamDirections)
            : seed {seed}
            , onStreamDirections {std::move (onStreamDirections)}
    {
    }

    void SynchroImpl::log (std::string const & msg) {
        // TODO: allow setting an external logging function
        std::cout << "Synchro @" << this << ": '" << msg << "'" << std::endl;
    }

    ::kj::Promise <void> SynchroImpl::ping (ConnectContext context) {
        log ("Listening");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::streamDirections (Synchro::Server::StreamDirectionsContext context) {
        log ("Add new projected spaceship");

        auto result = context.getResults();
        result.setCallback (kj::heap <DirectionCallbackImpl> (onStreamDirections()));

        auto param_client = context.getParams().getClient();
        if (param_client.isValue()) {
            auto request = param_client.getValue().streamDirectionsRequest ();
            request.initClient().setNothing ();
            request.send();
        }

        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::randomSeed (Synchro::Server::RandomSeedContext context) {
        context.initResults ().setSeed (seed);

        return kj::READY_NOW;
    }

}

/* Copyright © 2022 Aaron Alef */