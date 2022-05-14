#include "Spaceship.h"

namespace cg {
    Spaceship::Spaceship (Synchro::Spaceship::Reader reader)
            : username {reader.getUsername()}
            , position {reader.getPosition().getX(),
                        reader.getPosition().getY()}
            , velocity {reader.getVelocity().getX(),
                        reader.getVelocity().getY()}
            , angle {reader.getAngle()}
            {
            }

    Spaceship::Spaceship (std::string username, vector2 position, vector2 velocity, float angle)
            : username {std::move (username)}
            , position {position}
            , velocity {velocity}
            , angle {angle}
            {
            }

    void Spaceship::initialise (Synchro::Spaceship::Builder builder) const {
        builder.setUsername (username);

        auto pos = builder.initPosition();
        pos.setX (position[0]);
        pos.setY (position[1]);

        auto vel = builder.initVelocity();
        vel.setX (velocity[0]);
        vel.setY (velocity[1]);

        builder.setAngle (angle);
    }
}

/* Copyright © 2022 Aaron Alef */
