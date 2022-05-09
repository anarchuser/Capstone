#include "Server.h"

namespace cg {
    SynchroImpl::SynchroImpl (std::size_t seed)
            : rng_seed {seed}
    {
    }

    void SynchroImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Synchro @" << this << ": '" << msg << "'" << std::endl;
        KJ_LOG (INFO, ss.str());
    }

    ::kj::Promise <void> SynchroImpl::ping (PingContext context) {
        log ("Received Ping");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::seed (SeedContext context) {
        context.initResults().setSeed (rng_seed);
        return kj::READY_NOW;
    }

    ::kj::Promise<void> SynchroImpl::join (JoinContext context) {
        log ("Add new projected spaceship");

        auto maybe = context.getParams().getOther();
        if (maybe.isValue()) {
            log ("Establish connection back");

            auto client = maybe.getValue();

            // Ensure other client uses same seed
            client.seedRequest().send().then ([this] (capnp::Response <SeedResults> response) {
                KJ_ASSERT (response.getSeed() != rng_seed);
            });

            // TODO: do all the other join stuff
        }

        return kj::READY_NOW;
    }

}

/* Copyright © 2022 Aaron Alef */