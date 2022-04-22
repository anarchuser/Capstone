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

    void Dialog::addButton (std::string message, std::function <void (Event const *)> && callback) {
        ResFont * font = dialogResources.getResFont ("kt-liberation");
        spText msg = new Text (font, message);

        ResAnim * sprite = dialogResources.getResAnim ("button");
        spButton button = new Button (sprite, msg, std::forward <std::function <void (Event const *)>> (callback));
        addChild (button);
        buttons.push_back (button);
        button->setSize ({getSize().x - 200, 50});
        button->setPosition (getPosition().x + 100, getPosition().y + 100 + 100 * buttons.size());
    }
}

/* Copyright © 2022 Aaron Alef */
