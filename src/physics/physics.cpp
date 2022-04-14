#include "physics.h"

void ph::actor_preinit() {}

//called from main.cpp
void ph::actor_init() {
    ph::MainActor::gameResources.loadXML (PROJECT_ROOT "/src/physics/data/res.xml");

    spMainActor actor = new MainActor;
    getStage()->addChild(actor);
}

//called each frame from main.cpp
void ph::actor_update() {}

//called each frame from main.cpp
void ph::actor_destroy() {
    //free previously loaded resources
    ph::MainActor::gameResources.free();
}
