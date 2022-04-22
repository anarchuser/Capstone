#ifndef CAPSTONE_DIALOG_H
#define CAPSTONE_DIALOG_H

#include "config.h"
#include "Game/config.h"

#include "oxygine-framework.h"

namespace kt {
    using namespace oxygine;

    class Dialog: public Sprite {
    private:
        Resources dialogResources;

    public:
        Dialog (Vector2 pos, Vector2 size);
        ~Dialog () noexcept;

    };

    DECLARE_SMART (Dialog, spDialog);

} // kt

#endif //CAPSTONE_DIALOG_H

/* Copyright © 2022 Aaron Alef */
