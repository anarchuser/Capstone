#include "oxygine-framework.h"
#include "hello_world.h"

using namespace oxygine;

namespace hw {
    class MainActor : public Actor {
    public:
        spTextField _text;
        spSprite _button;

        static Resources gameResources;

        MainActor () {
            //create button Sprite
            spSprite button = new Sprite ();

            //setup it:
            //set button.png image. Resource 'button' defined in 'res.xml'
            button->setResAnim(gameResources.getResAnim("button"));

            //centered button at screen
            Vector2 pos = getStage ()->getSize () / 2 - button->getSize () / 2;
            button->setPosition (pos);

            //register  click handler to button
            EventCallback cb = CLOSURE(this, & MainActor::buttonClicked);
            button->addEventListener (TouchEvent::CLICK, cb);

            //animate mouse over and mouse out events
            cb = CLOSURE(this, & MainActor::buttonOverOut);
            button->addEventListener (TouchEvent::OVER, cb);
            button->addEventListener (TouchEvent::OUTX, cb);

#ifdef CLOSURE_FUNCTION //if your compiler supports lambda

            button->addEventListener (TouchEvent::CLICK, [] (Event * e) -> void {
                logs::messageln ("button clicked");
            });

#endif

            //attach button as child to current actor
            addChild (button);
            _button = button;



            //create TextField Actor
            spTextField text = new TextField ();
            //attach it as child to button
            text->attachTo (button);
            //centered in button
            text->setPosition (button->getSize () / 2);

            //initialize text style
            TextStyle style = TextStyle (gameResources.getResFont ("main")).withColor (Color::White).alignMiddle ();
            text->setStyle (style);
            text->setText ("Click\nMe!");

            _text = text;
        }

        void buttonClicked (Event * event) {
            //user clicked to button

            //animate button by changing color
            _button->setColor (Color::White);
            _button->addTween (Sprite::TweenColor (Color::Green), 500, 1, true);

            //animate text by scaling
            _text->setScale (1.0f);
            _text->addTween (Actor::TweenScale (1.1f), 500, 1, true);

            //and change text
            _text->setText ("Clicked!");

            //let's create and run sprite with simple animation
            runSprite ();
        }

        void buttonOverOut (Event * e) {
            if (e->type == TouchEvent::OVER) {
                _button->addTween (Sprite::TweenAddColor (Color (64, 64, 64, 0)), 300);
            }

            if (e->type == TouchEvent::OUTX) {
                _button->addTween (Sprite::TweenAddColor (Color (0, 0, 0, 0)), 300);
            }
        }

        void runSprite () {
            spSprite sprite = new Sprite ();
            addChild (sprite);

            int duration = 600;//ms
            int loops = -1;//infinity loops

            //animation has 8 columns - frames, check 'res.xml'
            ResAnim * animation = gameResources.getResAnim ("anim");

            //add animation tween to sprite
            //TweenAnim would change animation frames
            sprite->addTween (Sprite::TweenAnim (animation), duration, loops);

            Vector2 destPos = getStage ()->getSize () - sprite->getSize ();
            Vector2 srcPos = Vector2 (0, destPos.y);
            //set sprite initial position
            sprite->setPosition (srcPos);

            //add another tween: TweenQueue
            //TweenQueue is a collection of tweens
            spTweenQueue tweenQueue = new TweenQueue ();
            tweenQueue->setDelay (1500);
            //first, move sprite to dest position
            tweenQueue->add (Sprite::TweenPosition (destPos), 2500, 1);
            //then fade it out smoothly
            tweenQueue->add (Sprite::TweenAlpha (0), 500, 1);

            sprite->addTween (tweenQueue);

            //and remove sprite from tree when tweenQueue is empty
            //if you don't hold any references to sprite it would be deleted automatically
            tweenQueue->detachWhenDone ();
        }
    };

//declare spMainActor as intrusive_ptr holder of GameScene
typedef oxygine::intrusive_ptr<hw::MainActor> spMainActor;
//you could use DECLARE_SMART preprocessor definition it does the same:
//DECLARE_SMART(GameScene, spMainActor)


Resources hw::MainActor::gameResources = Resources();

void actor_preinit()
{
}

//called from main.cpp
void actor_init()
{
    //load xml file with resources definition
    hw::MainActor::gameResources.loadXML(PROJECT_ROOT "/examples/hello_world/data/res.xml");


    //let's create our client code simple actor
    //spMainActor was defined above as smart intrusive pointer (read more: http://www.boost.org/doc/libs/1_60_0/libs/smart_ptr/intrusive_ptr.html)
    spMainActor actor = new hw::MainActor;

    //and add it to Stage as child
    getStage()->addChild(actor);
}


//called each frame from main.cpp
void actor_update()
{
}

//called each frame from main.cpp
void actor_destroy()
{
    //free previously loaded resources
    hw::MainActor::gameResources.free();
}
}
