#include "CollisionEvent.h"

namespace kt {
    CollisionEvent::CollisionEvent (spSprite other, eventType type): Event (type), other {other} {}
}

/* Copyright © 2022 Aaron Alef */
