#include "Client.h"

namespace cg {
    kj::Promise <void> Client::erase (ShipName const & username) {
        // Remove the ship handle, if existing
        ships.erase (username);

        // Tell the sink the ship got destroyed, then remove it
        if (!sinks.contains (username)) return kj::READY_NOW;

        // Close the sink and then remove it from the map
        return sinks.at (username).doneRequest().send().ignoreResult().then ([this, username] () {
            sinks.erase (username);
        });
    }

    kj::Promise <void> Client::destroy () {
        ships.clear();

        kj::Vector <kj::Promise <void>> promises;
        for (auto & sink : sinks) {
            promises.add (erase (sink.first));
        }
        return kj::joinPromises (promises.releaseAsArray());
    }
}

/* Copyright © 2022 Aaron Alef */
