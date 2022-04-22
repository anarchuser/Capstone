#include "Dialog.h"

namespace kt {
    Dialog::Dialog (Vector2 pos, Vector2 size, std::string message) {
        setPosition (pos);
        setSize (size);
        setGuides (1, 1, 1, 1);

        dialogResources.loadXML (GAME_RESOURCES);
        setResAnim (dialogResources.getResAnim ("background"));

        spText text = new Text (dialogResources.getResFont ("kt-liberation"), message);
        text->setPosition ({pos.x + 100, pos.y + 100});
        addChild (text);
    }

    Dialog::~Dialog () noexcept {
        dialogResources.free();
    }

    void Dialog::setMessage (std::string message) {
        text->setText (message);
    }
}

/* Copyright © 2022 Aaron Alef */
