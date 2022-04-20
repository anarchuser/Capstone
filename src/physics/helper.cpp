#include "helper.h"

b2Vec2 ph::convert (oxygine::Vector2 const & pos) {
    return {pos.x / WORLD_SCALE, pos.y / WORLD_SCALE};
}

oxygine::Vector2 ph::convert (const b2Vec2 & pos) {
    return {pos.x * WORLD_SCALE, pos.y * WORLD_SCALE};
}

b2Vec2 ph::wrap (b2Vec2 pos) {
    auto const X = ox::getStage ()->getSize().x / WORLD_SCALE;
    auto const Y = ox::getStage ()->getSize().y / WORLD_SCALE;

    while (pos.x < 0) pos.x += X;
    while (pos.y < 0) pos.y += Y;
    pos.x = fmod (pos.x, X);
    pos.y = fmod (pos.y, Y);

    return pos;
}

/* Copyright © 2022 Aaron Alef */
