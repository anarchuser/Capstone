#include "click_box.h"

ox::Resources cb::MainActor::gameResources = Resources();

cb::MainActor::MainActor() {}

void cb::MainActor::buttonClicked(Event* event) {}

void cb::MainActor::buttonOverOut(Event* e) const {}

void cb::MainActor::runSprite() {}

//declare spMainActor as intrusive_ptr holder of cb::MainActor
typedef oxygine::intrusive_ptr<cb::MainActor> spMainActor;
//you could use DECLARE_SMART preprocessor definition it does the same:
//DECLARE_SMART(MainActor, spMainActor)

void cb::click_box_preinit() {}

//called from main.cpp
void cb::click_box_init()
{
    //let's create our client code simple actor
    //spMainActor was defined above as smart intrusive pointer (read more: http://www.boost.org/doc/libs/1_60_0/libs/smart_ptr/intrusive_ptr.html)
    spMainActor actor = new cb::MainActor;

    //and add it to Stage as child
    ox::getStage()->addChild(actor);

    ox::getStage()->addChild (new cb::Root);
}


//called each frame from main.cpp
void cb::click_box_update() {}

//called each frame from main.cpp
void cb::click_box_destroy() {
    //free previously loaded resources
    cb::MainActor::gameResources.free();
}

cb::Root::Root() {
    setSize(300, 200);
    setPosition(getStage()->getSize() / 2 - getSize() / 2);
    setColor(Color::Silver);
    spColorRectSprite box = new ColorRectSprite;
    box->setColor(Color::Salmon);
    box->setPosition(50, 50);
    box->setSize(100, 100);
    addChild(box);

    ox::getStage()->addEventListener (ox::TouchEvent::CLICK, CLOSURE(this, & Root::stageClick));
    box->addEventListener (ox::TouchEvent::CLICK, CLOSURE(this, & Root::boxClick));
    this->addEventListener (ox::TouchEvent::CLICK, CLOSURE(this, & Root::rootClick));
}

void cb::Root::boxClick (Event *) {
    logs::messageln("click: box");
}
void cb::Root::rootClick (Event *) {
    logs::messageln("click: root");
}
void cb::Root::stageClick (Event *) {
    logs::messageln("click: stage");
}

