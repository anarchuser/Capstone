#ifndef CAPSTONE_NETWORK_ITEM_H
#define CAPSTONE_NETWORK_ITEM_H

#include "Direction.h"
#include "Spaceship.h"

namespace cg {

    struct Item {
        Direction direction;
        Spaceship spaceship;

        Item() = default;
        explicit Item (Backend::Item::Reader reader);
        Item (Direction direction, Spaceship spaceship);

        void initialise (Backend::Item::Builder builder) const;
    };

} // cg

#endif //CAPSTONE_NETWORK_ITEM_H

/* Copyright © 2022 Aaron Alef */
