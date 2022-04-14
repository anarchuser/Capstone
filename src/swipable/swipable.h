#ifndef CP_SWIPABLE_H
#define CP_SWIPABLE_H

#include "oxygine-framework.h"

#include "config.h"

namespace sw {
    using namespace oxygine;

    class MainActor : public Actor {
    public:
        spTextField _text;
        spSprite _button;

        static Resources gameResources;

        MainActor ();

        void buttonClicked (Event * event);

        void buttonOverOut (Event * e) const;

        void runSprite ();
    };

    //declare spMainActor as intrusive_ptr holder of MainActor
    typedef oxygine::intrusive_ptr<sw::MainActor> spMainActor;
    //you could use DECLARE_SMART preprocessor definition it does the same:
    //DECLARE_SMART(MainActor, spMainActor)

    void actor_preinit();
    void actor_init();
    void actor_destroy();
    void actor_update();
    void actor_flush();

}

#endif//CP_SWIPABLE_H