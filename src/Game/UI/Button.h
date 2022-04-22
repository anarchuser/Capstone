#ifndef CAPSTONE_BUTTON_H
#define CAPSTONE_BUTTON_H

#include "config.h"

#include "Text.h"

#include "oxygine-framework.h"

#include <string>
#include <functional>

namespace kt {
    using namespace oxygine;

    class Button: public Box9Sprite {
    private:
        std::function <void (Event const *)> onAction;

        void onMouseOver (Event const * event);
        void onMouseOut  (Event const * event);

    public:
        Button (ResAnim * animation, spText message, std::function <void (Event const *)> && onAction);

        void trigger (Event const * event);
    };
    DECLARE_SMART (Button, spButton);

} // ky

#endif //CAPSTONE_BUTTON_H

/* Copyright © 2022 Aaron Alef */
