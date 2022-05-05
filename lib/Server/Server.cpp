#include "Server.h"

namespace cg {
    void SynchroImpl::log (std::string const & msg) {
        LOG (INFO) << "Synchro @" << this << ": '" << msg << "'";
        std::cout << "Synchro @" << this << ": '" << msg << "'" << std::endl;
    }

    ::kj::Promise <void> SynchroImpl::connect (ConnectContext context) {
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

    SynchroImpl::SynchroImpl (std::function <cg::DirectionCallback ()> && onStreamDirections)
            : onStreamDirections {std::move (onStreamDirections)} {}

}

/* Copyright © 2022 Aaron Alef */