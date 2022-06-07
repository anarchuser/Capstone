#include "Client.h"

namespace cg {
    void Client::erase (ShipName const & username) {
        // Remove the ship handle, if existing
        ships.erase (username);

        // Tell the sink the ship got destroyed, then remove it
        if (sinks.contains (username)) {
            auto sink = std::move (sinks.at (username));
            sinks.erase (username);
            sink.doneRequest().send().detach ([username](kj::Exception && e) {
                KJ_DLOG (WARNING, "Exception on closing sink to " + username, e.getDescription());
            });
        }
    }

    void Client::destroy () {
        ships.clear();
        for (auto const & sink : sinks) {
            erase (sink.first);
        }
    }
}

/* Copyright © 2022 Aaron Alef */
