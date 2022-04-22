#include "Dialog.h"

namespace kt {
    Dialog::Dialog (Vector2 pos, Vector2 size) {
        setPosition (pos);
        setSize (size);
        setScale (size);

        dialogResources.loadXML (GAME_RESOURCES);

        setResAnim (dialogResources.getResAnim ("background"));
    }

    Dialog::~Dialog () noexcept {
        dialogResources.free();
    }
}

/* Copyright © 2022 Aaron Alef */
