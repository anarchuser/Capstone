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

        context.getResults().setSynchro (kj::heap <Backend::Synchro::Server> ());
        return kj::READY_NOW;
    }

    ::kj::Promise <void> BackendImpl::join (JoinContext context) {
        log ("Join request received");
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */