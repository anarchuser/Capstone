#include "CollisionEvent.h"

namespace kt {
    CollisionEvent::CollisionEvent (std::pair <spActor, spActor> actors, eventType type): Event (type), actors {actors} {}
}

/* Copyright © 2022 Aaron Alef */
