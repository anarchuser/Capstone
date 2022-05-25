#include "Backend.h"

namespace cg {
    BackendImpl::BackendImpl (std::size_t seed) : rng_seed {seed} {}

    void BackendImpl::log (std::string const & msg) {
        std::stringstream ss;
        ss << "Backend @" << this << ": '" << msg << "'";
        KJ_DLOG (INFO, ss.str());
        std::cout << ss.str() << std::endl;
    }

    ::kj::Promise <void> BackendImpl::ping (PingContext context) {
        log ("Ping");
        return kj::READY_NOW;
    }

    ::kj::Promise<void> BackendImpl::seed (SeedContext context) {
        log ("RNG Seed (" + std::to_string (rng_seed) + ") requested");
        context.initResults().setSeed (rng_seed);
        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::connect (ConnectContext context) {
        log ("Connect request received");

        auto params = context.getParams();
        KJ_REQUIRE (params.hasRegistrar());
        clients.push_back (params.getRegistrar());
        log ("Number of clients connected: "s += std::to_string (clients.size()));

        auto results = context.getResults();
        auto registrar = kj::heap <RegistrarImpl>();
        registrar->setOnRegisterShip ([this] () {
            log ("Register Ship Callback");
        });
        results.setRegistrar (kj::mv (registrar));

        auto synchro = kj::heap <SynchroImpl>();
        results.setSynchro (kj::mv (synchro));

        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        log ("Join request received");

        auto params = context.getParams();
        KJ_REQUIRE (params.hasRemote());
        synchros.push_back (params.getRemote());
        log ("Number of synchros connected: "s += std::to_string (synchros.size()));

        auto results = context.getResults();
        results.setLocal (kj::heap <SynchroImpl> ());

        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */