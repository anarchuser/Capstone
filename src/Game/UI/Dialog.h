#ifndef CAPSTONE_DIALOG_H
#define CAPSTONE_DIALOG_H

#include "config.h"
#include "Game/config.h"

#include "Button.h"
#include "Text.h"

#include "oxygine-framework.h"

#include <string>
#include <functional>

namespace kt {
    using namespace oxygine;

    /// Class used to create a dialog window.
    /// Consists of a message and a list of buttons that get appended below the message
    class Dialog: public Box9Sprite {
    private:
        /// Font and background of the dialog
        static Resources dialogResources;
        /// Text field displayed in the dialog
        spText text;
        /// List of buttons displayed
        std::vector <spButton> buttons;

    public:
        /// Generate a new dialog of given size at given position with given message. Buttons get added later
        Dialog (Vector2 pos, Vector2 size, std::string message = "");
        ~Dialog () noexcept override;

        /// Replace the dialog message
        void setMessage (std::string message);
        /// Append another button to the dialog with a callback to call upon press
        void addButton (std::string message, std::function <void (Event *)> && callback);
    };

    DECLARE_SMART (Dialog, spDialog);

} // kt

#endif //CAPSTONE_DIALOG_H

/* Copyright © 2022 Aaron Alef */
