#include "RemoteSpaceship.h"

namespace kt {
    // TODO: ping to one specific server instance?
    RemoteSpaceship::RemoteSpaceship (World & world, Resources * res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale)
    {
        setAddColor (REMOTE_SPACESHIP_COLOR);
    }

    void RemoteSpaceship::destroy () {
        Spaceship::destroy();
        updateScoreboard ("ghost");
    }

    kj::Own <cg::ShipHandleImpl> RemoteSpaceship::getHandle () {
        setAwake (true);

        auto handle = kj::heap <cg::ShipHandleImpl> ();
        handle->setOnDone         (CLOSURE (this, & RemoteSpaceship::destroy));
        handle->setOnSendItem     (CLOSURE (this, & RemoteSpaceship::updateDirection));
        handle->setOnGetSpaceship (CLOSURE (this, & RemoteSpaceship::getData));
        handle->setOnSetSpaceship (CLOSURE (this, & RemoteSpaceship::setData));
        return handle;
    }
}

/* Copyright © 2022 Aaron Alef */
