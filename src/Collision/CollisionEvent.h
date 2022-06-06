#ifndef CAPSTONE_COLLISIONEVENT_H
#define CAPSTONE_COLLISIONEVENT_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include <utility>

namespace kt {
    using namespace oxygine;

    /// Event called onto a sprite when it collides with a different sprite
    class CollisionEvent: public Event {
    public:
        /// Different event types propagated by this class. Based on the four different event types box2d creates
        enum {
            /// Two objects touch
            BEGIN = eventID ('C', 'o', 'B', 'e'),
            /// Two objects stop touching
            END   = eventID ('C', 'o', 'E', 'n'),
            /// Two objects come into close range
            PRE   = eventID ('C', 'o', 'P', 'r'),
            /// Two objects have collided
            POST  = eventID ('C', 'o', 'P', 'o')
        };

        /// Object that has been collided with
        spSprite other;

        /// Create a new collision event of given type with given object. Can then be dispatched onto the collidee
        CollisionEvent (spSprite const & other, eventType type);
    };

} // kt

#endif //CAPSTONE_COLLISIONEVENT_H

/* Copyright © 2022 Aaron Alef */
