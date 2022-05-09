#include "Server.h"

namespace cg {
    SynchroImpl::SynchroImpl (std::size_t seed)
            : seed {seed}
    {
    }

    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro @" << this << ": '" << msg << "'" << std::endl;
        KJ_LOG (INFO, ss.str());
    }

    void SynchroImpl::setOnJoin (JoinCallback onJoin) {
        this->onJoin.exchange (onJoin);
    }

    ::kj::Promise <void> SynchroImpl::ping (PingContext context) {
        log ("Received Ping");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::randomSeed (RandomSeedContext context) {
        context.initResults().setSeed (seed);
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::join (JoinContext context) {
        log ("Add new projected spaceship");

        auto maybe = context.getParams().getOther();
        if (maybe.isValue()) {
            log ("Establish connection back");

            auto client = maybe.getValue();

            // Ensure other client uses same seed
            client.randomSeedRequest().send().then ([this] (capnp::Response <RandomSeedResults> response) {
                KJ_ASSERT (response.getSeed() != seed);
            });

            auto retour = client.joinRequest();
            retour.initOther().setNothing();

            kj::Own <CallbackDirectionImpl> impl;
            auto callback_ptr = onJoin.load();
            if (! callback_ptr.expired()) {
                auto callbacks = (* callback_ptr.lock())();
                impl = kj::heap <CallbackDirectionImpl> (std::move (callbacks));
            } else {
                impl = kj::heap <CallbackDirectionImpl> ();
            }
            retour.setCallback (kj::mv (impl));
            retour.send();
        }

        callbacks.push_back (context.getParams().getCallback());

        return kj::READY_NOW;
    }

}

/* Copyright © 2022 Aaron Alef */