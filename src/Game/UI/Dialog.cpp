#include "Dialog.h"

namespace kt {
    Dialog::Dialog (Vector2 pos, Vector2 size, std::string message) {
        setPosition (pos);
        setSize (size);
        setGuides (1, 1, 1, 1);

        dialogResources.loadXML (GAME_RESOURCES);
        setResAnim (dialogResources.getResAnim ("background"));
    }

    Dialog::~Dialog () noexcept {
        dialogResources.free();
    }
}

/* Copyright © 2022 Aaron Alef */
