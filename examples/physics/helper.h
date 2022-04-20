#ifndef CAPSTONE_HELPER_H
#define CAPSTONE_HELPER_H

#include "oxygine-framework.h"
#include "box2d.h"

#include "game_config.h"

namespace ph {
    using namespace oxygine;

    b2Vec2 convert (Vector2 const & pos);

    Vector2 convert (const b2Vec2 & pos);

    b2Vec2 wrap (b2Vec2 pos);
}
#endif //CAPSTONE_HELPER_H

/* Copyright © 2022 Aaron Alef */
