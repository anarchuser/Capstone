#include "Item.h"

namespace cg {
    Item::Item (int timestamp, Direction direction, Spaceship spaceship)
            : timestamp {timestamp}
            , direction {direction}
            , spaceship {std::move (spaceship)}
            {
            }

    Item::Item (Backend::Item::Reader reader)
            : timestamp {reader.getTimestamp()}
            , direction {reader.getDirection()}
            , spaceship {reader.getSpaceship()}
            {
            }

    void Item::initialise (Backend::Item::Builder builder) const {
        builder.setTimestamp (timestamp);
        direction.initialise (builder.initDirection());
        spaceship.initialise (builder.initSpaceship());
    }
}

/* Copyright © 2022 Aaron Alef */
