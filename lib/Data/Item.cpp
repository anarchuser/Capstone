#include "Item.h"

namespace cg {
    Item::Item (Direction direction, Spaceship spaceship)
            : direction {direction}
            , spaceship {std::move (spaceship)}
            {
            }

    Item::Item (Backend::Item::Reader reader)
            : direction {reader.getDirection()}
            , spaceship {reader.getSpaceship()}
            {
            }

    void Item::initialise (Backend::Item::Builder builder) const {
        direction.initialise (builder.initDirection());
        spaceship.initialise (builder.initSpaceship());
    }
}

/* Copyright © 2022 Aaron Alef */
