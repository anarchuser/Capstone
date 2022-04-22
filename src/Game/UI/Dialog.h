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

    class Dialog: public Box9Sprite {
    private:
        static Resources dialogResources;
        spText text;
        std::vector <spButton> buttons;

    public:
        Dialog (Vector2 pos, Vector2 size, std::string message = "");
        ~Dialog () noexcept override;

        void setMessage (std::string message);
        void addButton (std::string message, std::function <void (Event *)> && callback);
    };

    DECLARE_SMART (Dialog, spDialog);

} // kt

#endif //CAPSTONE_DIALOG_H

/* Copyright © 2022 Aaron Alef */
