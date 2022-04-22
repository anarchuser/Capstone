#ifndef CAPSTONE_DIALOG_H
#define CAPSTONE_DIALOG_H

#include "config.h"
#include "Game/config.h"

#include "Button.h"
#include "Text.h"

#include "oxygine-framework.h"

#include <string>

namespace kt {
    using namespace oxygine;

    class Dialog: public Box9Sprite {
    private:
        Resources dialogResources;
        spText text;

    public:
        Dialog (Vector2 pos, Vector2 size, std::string message = "");
        ~Dialog () noexcept override;

        void setMessage (std::string message);
        void addButton (spButton button);
    };

    DECLARE_SMART (Dialog, spDialog);

} // kt

#endif //CAPSTONE_DIALOG_H

/* Copyright © 2022 Aaron Alef */
