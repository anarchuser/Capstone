#ifndef CAPSTONE_COLLISIONEVENT_H
#define CAPSTONE_COLLISIONEVENT_H

#include "config.h"

#include "oxygine-framework.h"
#include "box2d.h"

#include <utility>

namespace kt {
    using namespace oxygine;

    class CollisionEvent: public Event {
    public:
        enum { BEGIN = eventID ('C', 'o', 'l', 'l') };

        std::pair <spActor, spActor> const actors;

        CollisionEvent (std::pair <spActor, spActor> actors, eventType type);
    };

} // kt

#endif //CAPSTONE_COLLISIONEVENT_H

/* Copyright © 2022 Aaron Alef */
