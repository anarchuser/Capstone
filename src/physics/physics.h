#ifndef CP_PHYSICS_H
#define CP_PHYSICS_H

#include "config.h"

#include <memory>

#include "oxygine-framework.h"
#include "box2d.h"

#include "MainActor/MainActor.h"

namespace ph {

    using namespace oxygine;

    void actor_preinit();
    void actor_init();
    void actor_destroy();
    void actor_update();
    void actor_flush();

    void onEvent (Event * ev);
}

#endif//CP_PHYSICS_H