#ifndef CP_SWIPABLE_H
#define CP_SWIPABLE_H

#include "oxygine-framework.h"

#include "config.h"

namespace cb {
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

    struct Root : public ColorRectSprite {
        void boxClick(Event *);
        void rootClick(Event *);
        void stageClick(Event *);
        Root();
    };

    //declare spMainActor as intrusive_ptr holder of MainActor
    typedef oxygine::intrusive_ptr<cb::MainActor> spMainActor;
    //you could use DECLARE_SMART preprocessor definition it does the same:
    //DECLARE_SMART(MainActor, spMainActor)

    void swipable_preinit();
    void swipable_init();
    void swipable_destroy();
    void swipable_update();
    void swipable_flush();

}

#endif//CP_SWIPABLE_H