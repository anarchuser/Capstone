#include "physics.h"

ox::Resources ph::MainActor::gameResources = Resources();

ph::MainActor::MainActor() = default;

//declare spMainActor as intrusive_ptr holder of ph::MainActor
typedef oxygine::intrusive_ptr<ph::MainActor> spMainActor;
//you could use DECLARE_SMART preprocessor definition it does the same:
//DECLARE_SMART(MainActor, spMainActor)

void ph::actor_preinit() {}

//called from main.cpp
void ph::actor_init() {
    ph::MainActor::gameResources.loadXML (PROJECT_ROOT "/src/physics/data/res.xml");
}


//called each frame from main.cpp
void ph::actor_update() {}

//called each frame from main.cpp
void ph::actor_destroy() {
    //free previously loaded resources
    ph::MainActor::gameResources.free();
}
