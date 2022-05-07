#include "Dialog.h"

namespace kt {
    Resources Dialog::dialogResources;

    Dialog::Dialog (Vector2 pos, Vector2 size, std::string message) {
        setPosition (pos);
        setSize (size);
        setGuides (1, 1, 1, 1);

        dialogResources.loadXML (GAME_RESOURCES);
        setResAnim (dialogResources.getResAnim ("dialog"));

        spText text = new Text (dialogResources.getResFont ("kt-liberation"), message);
        text->setHAlign (TextStyle::HALIGN_MIDDLE);
        text->setPosition (size.x / 2, 50);
        addChild (text);
    }

    Dialog::~Dialog () noexcept {
        dialogResources.free();
    }

    void Dialog::setMessage (std::string message) {
        text->setText (message);
    }

    void Dialog::addButton (std::string message, std::function <void (Event *)> && callback) {
        ResFont * font = dialogResources.getResFont ("kt-liberation");
        spText msg = new Text (font, message);

        ResAnim * sprite = dialogResources.getResAnim ("button");
        spButton button = new Button (sprite, msg, std::forward <std::function <void (Event *)>> (callback));
        button->setSize ({0.8f * getSize().x, 50});
        button->setPosition (0.1f * getSize().x, getSize().y / 5 + std::max (getSize().y / 10, button->getSize().y + 10) * buttons.size());
        addChild (button);
        buttons.emplace_back (button);
    }
    void Dialog::addInput (std::string default_value, std::function <void (std::string)> && callback) {
        ResFont * font = dialogResources.getResFont ("kt-liberation");
        spText msg = new Text (font, default_value);

        ResAnim * sprite = dialogResources.getResAnim ("input");
        spInput input = new Input (sprite, msg, std::forward <std::function <void (std::string)>> (callback));
        input->setSize ({0.8f * getSize().x, 50});
        input->setPosition (0.1f * getSize().x, getSize().y / 5 + std::max (getSize().y / 10, input->getSize().y + 10) * buttons.size());
        addChild (input);
        buttons.emplace_back (input);
    }
}

/* Copyright © 2022 Aaron Alef */
