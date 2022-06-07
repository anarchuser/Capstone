#ifndef CAPSTONE_DIALOG_H
#define CAPSTONE_DIALOG_H

#include "config.h"

#include "Input.h"
#include "Button.h"
#include "Text.h"

#include "oxygine-framework.h"

#include <string>
#include <functional>

namespace kt {
    using namespace oxygine;

    /// Interactive dialog window with description and list of buttons / input fields
    class Dialog: public Box9Sprite {
    private:
        /// Font and background of the dialog
        Resources dialogResources;
        /// Text field displayed in the dialog
        spText text;
        /// List of buttons displayed
        std::vector <spSprite> buttons;

    public:
        /// Generate a new dialog of given size at given position with given message. Buttons get added later
        Dialog (Vector2 pos, Vector2 size, std::string message = "");
        /// Free resources on destruction
        ~Dialog () noexcept override;

        /// Append another button to the dialog with a callback to call upon press
        void addButton (std::string const & message, std::function <void (Event *)> && callback);
        /// Append another button to the dialog with a callback to call upon press
        void addInput (std::string const & default_value, std::function <void (std::string)> && callback);
    };
    DECLARE_SMART (Dialog, spDialog);

} // kt

#endif //CAPSTONE_DIALOG_H

/* Copyright © 2022 Aaron Alef */
