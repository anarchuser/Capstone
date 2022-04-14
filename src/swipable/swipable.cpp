#include "swipable.h"

ox::Resources sw::MainActor::gameResources = Resources();

sw::MainActor::MainActor() {}

void sw::MainActor::buttonClicked(Event* event) {}

void sw::MainActor::buttonOverOut(Event* e) const {}

void sw::MainActor::runSprite() {}

//declare spMainActor as intrusive_ptr holder of sw::MainActor
typedef oxygine::intrusive_ptr<sw::MainActor> spMainActor;
//you could use DECLARE_SMART preprocessor definition it does the same:
//DECLARE_SMART(MainActor, spMainActor)

void sw::actor_preinit() {}

//called from main.cpp
void sw::actor_init()
{
    //let's create our client code simple actor
    //spMainActor was defined above as smart intrusive pointer (read more: http://www.boost.org/doc/libs/1_60_0/libs/smart_ptr/intrusive_ptr.html)
    spMainActor actor = new sw::MainActor;

    //and add it to Stage as child
    ox::getStage()->addChild(actor);
}


//called each frame from main.cpp
void sw::actor_update() {}

//called each frame from main.cpp
void sw::actor_destroy() {
    //free previously loaded resources
    sw::MainActor::gameResources.free();
}
