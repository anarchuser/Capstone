#ifndef CAPSTONE_INPUT_H
#define CAPSTONE_INPUT_H

#include "config.h"

#include "oxygine-framework.h"
#include "Button.h"

#include <string>

namespace kt {
    using namespace oxygine;

    class Input: public Sprite {
    private:
        /// Callback function to call upon press
        std::function <void (std::string)> onEnter;

        /// Click on field
        void onClick (Event * event);
        /// Mouse over animation (highlight field)
        void onMouseOver (Event * event);
        /// Mouse out animation (revert highlight)
        void onMouseOut  (Event * event);

    public:
        Input (ResAnim * animation, spText default_value, std::function <void (std::string)> && onEnter);

    };

} // kt

#endif //CAPSTONE_INPUT_H

/* Copyright © 2022 Aaron Alef */
