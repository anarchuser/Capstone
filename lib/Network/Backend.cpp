#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed, ClientID id)
            : rng_seed {seed}
            , ID {std::move (id)}
            , local {}
            , remotes {}
            , synchro (ID, local, remotes)
            {}

    void BackendImpl::log (std::string const & msg) const {
        std::ostringstream ss;
        ss << "Backend {" << ID << "} @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        debug_stdout (ss.str());
    }

    ::kj::Promise <void> BackendImpl::ping (PingContext context) {
        return kj::READY_NOW;
    }

    ::kj::Promise<void> BackendImpl::seed (SeedContext context) {
        log ("RNG Seed (" + std::to_string (rng_seed) + ") requested");
        context.initResults().setSeed (rng_seed);
        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::connect (ConnectContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        ClientID const & remoteID = params.getId();
        log ("Connect request received from local client " + ClientID (params.getId()));
        KJ_REQUIRE (remoteID == ID, remoteID, "Local client ID must have our backend ID");
        KJ_REQUIRE (!local, "A local client is registered already");

        KJ_REQUIRE (params.hasRegistrar());

        local.emplace (params.getRegistrar());

        auto results = context.getResults();
        results.setId (ID);
        results.setRegistrar (synchro.newRegistrar (ID));
        results.setSynchro   (synchro.newSynchro   (ID));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        auto params = context.getParams();
        KJ_REQUIRE (params.hasId());
        ClientID const & remoteID = params.getId();
        log ("Join request received from " + remoteID);
        KJ_REQUIRE (remoteID != ID, remoteID, "Remote client cannot have our own identifier");

        auto promises = kj::heapArrayBuilder <kj::Promise <void>> (2);

        // Share our own synchro callback to new connection
        KJ_REQUIRE (params.hasRemote());
        promises.add (synchro.shareConnections (remoteID, params.getRemote()));

        // Connect back to received synchro
        promises.add (synchro.connectTo (remoteID, params.getRemote()));

        return kj::joinPromises (promises.finish());
    }
}

/* Copyright © 2022 Aaron Alef */