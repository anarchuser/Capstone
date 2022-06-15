#ifndef CAPSTONE_NETWORK_ITEM_H
#define CAPSTONE_NETWORK_ITEM_H

#include "Direction.h"
#include "Spaceship.h"

namespace cg {

    /// Item struct in correspondence to Backend::Item
    struct Item {
        /// The in-game time when this item was constructed
        int timestamp;
        /// The direction sent with this item
        Direction direction;
        /// The status of the spaceship when it sent this
        Spaceship spaceship;

        /// Construct a new item from the given parameters
        Item (int timestamp, Direction direction, Spaceship spaceship);
        /// Construct a new item from the given Item::Reader
        explicit Item (Backend::Item::Reader reader);

        /// Use this item to initialise an Item::Builder object
        void initialise (Backend::Item::Builder builder) const;
    };

} // cg

#endif //CAPSTONE_NETWORK_ITEM_H

/* Copyright © 2022 Aaron Alef */
