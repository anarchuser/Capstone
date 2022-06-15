#include "Client.h"

namespace cg {
    kj::Promise <void> Client::distributeSpaceship (Spaceship const & ship, ShipHandle_t handle) {
        auto & username = ship.username;
        if (sinks.contains (username)) return kj::READY_NOW;

        auto request = registrar.registerShipRequest ();
        ship.initialise (request.initShip ());
        request.setHandle (handle);
        return request.send().then ([this, username] (auto results) {
            // Check again if nothing's changed
            if (sinks.contains (username)) return;
            sinks.emplace (username, results.getSink ());
        });
    }

    kj::Promise <void> Client::sendItemToClient (Item const & item, ShipHandle_t handle) {
        auto const & [time, direction, data] = item;
        auto const & username = data.username;

        if (!sinks.contains (username)) {
            // No sink exists yet -> request one
            return handle.getShipRequest().send()
                    .then ([this, handle] (auto results) mutable {
                        return distributeSpaceship (Spaceship (results.getShip()), handle);
                    });
        }

        auto request = sinks.at (username).sendItemRequest();
        request.initItem().setTimestamp (time);
        direction.initialise (request.getItem().initDirection());
        data.initialise (request.getItem().initSpaceship());
        return request.send().ignoreResult()
                .catch_ ([this, & username] (kj::Exception && e) {
                    sinks.erase (username);
                });
    }

    kj::Promise <void> Client::erase (ShipName const & username) {
        // Remove the ship handle, if existing
        ships.erase (username);

        // Tell the sink the ship got destroyed, then remove it
        if (!sinks.contains (username)) return kj::READY_NOW;

        // Close the sink and then remove it from the map
        return sinks.at (username).doneRequest().send().ignoreResult()
                .then ([this, username] () {
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
