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
void sw::actor_init() {
    sw::MainActor::gameResources.loadXML (PROJECT_ROOT "/src/swipable/data/res.xml");

    ox::spSprite sprite = new ox::Sprite;
    sprite->setResAnim (sw::MainActor::gameResources.getResAnim ("img1"));
    sprite->attachTo (ox::getStage());
}


//called each frame from main.cpp
void sw::actor_update() {}

//called each frame from main.cpp
void sw::actor_destroy() {
    //free previously loaded resources
    sw::MainActor::gameResources.free();
}
