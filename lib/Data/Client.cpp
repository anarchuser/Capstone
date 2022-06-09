#include "Client.h"

namespace cg {
    kj::Promise <void> Client::erase (ShipName const & username) {
        // Remove the ship handle, if existing
        ships.erase (username);

        // Tell the sink the ship got destroyed, then remove it
        if (!sinks.contains (username)) return kj::READY_NOW;

        auto promise = sinks.at (username).doneRequest().send().ignoreResult();
        sinks.erase (username);
        return promise;
    }

    kj::Promise <void> Client::destroy () {
        ships.clear();

        auto promises = kj::heapArrayBuilder <kj::Promise <void>> (sinks.size());
        for (auto & sink : sinks) {
            promises.add (erase (sink.first));
        }
        return kj::joinPromises (promises.finish());
    }
}

/* Copyright © 2022 Aaron Alef */
