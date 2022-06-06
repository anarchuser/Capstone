#include "Client.h"

namespace cg {
    kj::Promise <void> Client::erase (ShipName const & username) {
        // Remove the ship handle, if existing
        ships.erase (username);

        // Tell the sink the ship got destroyed, then remove it
        if (sinks.contains (username)) {
            return sinks.at (username).doneRequest().send().ignoreResult().then ([&] () {
                sinks.erase (username);
            });
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
