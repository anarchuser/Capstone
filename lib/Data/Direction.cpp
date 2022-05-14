#include "Direction.h"

namespace cg {
    Direction::Direction (Synchro::Direction::Reader reader)
            : accelerate (reader.getAccelerate())
            , decelerate (reader.getDecelerate())
            , rotateLeft (reader.getRotateLeft())
            , rotateRight (reader.getRotateRight())
            {
            }

    void Direction::initialise (Synchro::Direction::Builder builder) const {
        builder.setAccelerate (accelerate);
        builder.setDecelerate (decelerate);
        builder.setRotateLeft (rotateLeft);
        builder.setRotateRight (rotateRight);
    }
}

/* Copyright © 2022 Aaron Alef */
