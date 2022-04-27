#ifndef CAPSTONE_BUTTON_H
#define CAPSTONE_BUTTON_H

#include "config.h"

#include "Text.h"

#include "oxygine-framework.h"

#include <string>
#include <functional>

namespace kt {
    using namespace oxygine;

    /// Button consisting of a description. Calls `onAction` on press
    class Button: public Sprite {
    private:
        /// Callback function to call upon press
        std::function <void (Event *)> onAction;

        /// Mouse over animation (highlight button)
        void onMouseOver (Event * event);
        /// Mouse out animation (revert highlight)
        void onMouseOut  (Event * event);

    public:
        /// Create a new button with given sprite, text field and callback
        Button (ResAnim * animation, spText message, std::function <void (Event *)> && onAction);

        /// Trigger callback
        void trigger (Event * event);
    };
    DECLARE_SMART (Button, spButton);

} // ky

#endif //CAPSTONE_BUTTON_H

/* Copyright © 2022 Aaron Alef */
