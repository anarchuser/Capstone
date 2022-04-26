#include "swipable.h"

ox::Resources sw::MainActor::gameResources = Resources();

sw::MainActor::MainActor() {}

//declare spMainActor as intrusive_ptr holder of sw::GameScene
typedef oxygine::intrusive_ptr<sw::MainActor> spMainActor;
//you could use DECLARE_SMART preprocessor definition it does the same:
//DECLARE_SMART(GameScene, spMainActor)

void sw::actor_preinit() {}

//called from main.cpp
void sw::actor_init() {
    sw::MainActor::gameResources.loadXML (PROJECT_ROOT "/src/swipable/data/res.xml");

    static bool pressed = false;
    static Vector2 downPos(0, 0);
    static int current = 0;
    static spSprite sprite;
    static char const * images[] = {"img1", "img2", "img3", "img4"};

    sprite = new ox::Sprite;
    sprite->setResAnim (sw::MainActor::gameResources.getResAnim ("img1"));
    sprite->attachTo (ox::getStage());

    getStage()->addEventListener (ox::TouchEvent::TOUCH_DOWN, [=](Event * ev) {
        auto * touch = (TouchEvent *) ev;
        downPos = touch->localPosition;
        pressed = true;
    });
    getStage()->addEventListener (ox::TouchEvent::TOUCH_UP, [=](Event *) {
        pressed = false;
    });

    getStage()->addEventListener (oxygine::TouchEvent::MOVE, [=](Event * ev) {
        if (!pressed) return;

        auto * touch = (TouchEvent *) ev;
        Vector2 dir = touch->localPosition - downPos;
        if (dir.x < -50) {
            pressed = false;

            sprite->addTween(Actor::TweenX(-sprite->getWidth()), 300)->detachWhenDone ();
            current = (current + 1) % 4;

            spSprite nextSprite = new Sprite;
            nextSprite->setResAnim (sw::MainActor::gameResources.getResAnim (images [current]));
            nextSprite->setX (sprite->getWidth());
            nextSprite->addTween (Actor::TweenX(0), 300);
            nextSprite->attachTo (getStage());
            sprite = nextSprite;
        }
        if (dir.x > 50) {
            pressed = false;

            sprite->addTween(Actor::TweenX(sprite->getWidth()), 300)->detachWhenDone ();
            current = (current + 3) % 4;

            spSprite nextSprite = new Sprite;
            nextSprite->setResAnim (sw::MainActor::gameResources.getResAnim (images [current]));
            nextSprite->setX (-sprite->getWidth());
            nextSprite->addTween (Actor::TweenX(0), 300);
            nextSprite->attachTo (getStage());
            sprite = nextSprite;
        }
    });
}


//called each frame from main.cpp
void sw::actor_update() {}

//called each frame from main.cpp
void sw::actor_destroy() {
    //free previously loaded resources
    sw::MainActor::gameResources.free();
}
