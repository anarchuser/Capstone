#include "Client.h"

namespace cg {
    kj::Promise <void> Client::erase (ShipName const & username) {
        ships.erase (username);

        if (sinks.contains (username)) {
            return sinks.at (username).doneRequest().send().ignoreResult().then ([&] () {
                sinks.erase (username);
            });
        }
        return kj::READY_NOW;
    }
}

/* Copyright © 2022 Aaron Alef */
