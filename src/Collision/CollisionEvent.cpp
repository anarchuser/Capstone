#include "CollisionEvent.h"

namespace kt {
    CollisionEvent::CollisionEvent (spSprite const & other, eventType type): Event (type), other {other} {}
}

/* Copyright © 2022 Aaron Alef */
