#ifndef CAPSTONE_INPUT_H
#define CAPSTONE_INPUT_H

#include "config.h"

#include "oxygine-framework.h"
#include "Button.h"

#include <string>

namespace kt {
    using namespace oxygine;

    /// Input field. Only supports alphanumerical characters and dots
    class Input: public Sprite {
    private:
        /// The listener used to capture key presses. Attached on click, detached when losing focus
        int listener;

        /// Callback function to call upon press
        std::function <void (std::string)> onEnter;

        /// Text field mirroring whatever has been typed
        spText message;

        /// On click, execute enEnter callback
        void onClick (Event * event);
        /// Mouse over animation (highlight field)
        void onMouseOver (Event * event);
        /// Mouse out animation (revert highlight)
        void onMouseOut  (Event * event);

    public:
        /// Construct new Input field with given sprite, text field and callback
        Input (ResAnim * animation, spText default_value, std::function <void (std::string)> && onEnter);

    };
    DECLARE_SMART (Input, spInput);

} // kt

#endif //CAPSTONE_INPUT_H

/* Copyright © 2022 Aaron Alef */
