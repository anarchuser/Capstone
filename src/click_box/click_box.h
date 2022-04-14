#ifndef CP_CLICK_BOX_H
#define CP_CLICK_BOX_H

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

    void click_box_preinit();
    void click_box_init();
    void click_box_destroy();
    void click_box_update();
    void click_box_flush();

}

#endif//CP_CLICK_BOX_H