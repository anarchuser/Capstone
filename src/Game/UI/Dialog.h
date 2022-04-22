#ifndef CAPSTONE_DIALOG_H
#define CAPSTONE_DIALOG_H

#include "config.h"
#include "Game/config.h"

#include "oxygine-framework.h"

#include <string>

namespace kt {
    using namespace oxygine;

    class Dialog: public Box9Sprite {
    private:
        Resources dialogResources;

    public:
        std::string const message;

        Dialog (Vector2 pos, Vector2 size, std::string message = "");
        ~Dialog () noexcept;

    };

    DECLARE_SMART (Dialog, spDialog);

} // kt

#endif //CAPSTONE_DIALOG_H

/* Copyright © 2022 Aaron Alef */
