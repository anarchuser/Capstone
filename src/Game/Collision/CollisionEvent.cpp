#include "CollisionEvent.h"

namespace kt {
    CollisionEvent::CollisionEvent (std::pair <spActor, spActor> actors): Event (EVENT), actors {actors} {}
}

/* Copyright © 2022 Aaron Alef */
