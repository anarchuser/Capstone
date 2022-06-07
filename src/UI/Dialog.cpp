#include "Dialog.h"

namespace kt {
    Dialog::Dialog (Vector2 pos, Vector2 size, std::string message) {
        setPosition (pos);
        setSize (size);
        setGuides (1, 1, 1, 1);

        // Load and use dialog sprite
        dialogResources.loadXML (GAME_RESOURCES);
        setResAnim (dialogResources.getResAnim ("dialog"));

        // Put the message on top of the dialog
        spText text = new Text (dialogResources.getResFont ("kt-liberation"), message);
        text->setHAlign (TextStyle::HALIGN_MIDDLE);
        text->setPosition (size.x / 2, 50);
        addChild (text);
    }

    Dialog::~Dialog () noexcept {
        // Free the used resources
        dialogResources.free();
    }

    void Dialog::addButton (std::string const & message, std::function <void (Event *)> && callback) {
        // Generate a new text field for the button
        ResFont * font = dialogResources.getResFont ("kt-liberation");
        spText msg = new Text (font, message);

        // Create a new button to attach the text to
        ResAnim * sprite = dialogResources.getResAnim ("button");
        spButton button = new Button (sprite, msg, std::forward <std::function <void (Event *)>> (callback));
        button->setSize ({0.8f * getSize().x, 50});
        button->setPosition (0.1f * getSize().x, getSize().y / 5 + std::max (getSize().y / 10, button->getSize().y + 10) * buttons.size());
        addChild (button);
        buttons.emplace_back (button);
    }
    void Dialog::addInput (std::string const & default_value, std::function <void (std::string)> && callback) {
        // Generate a new text field for the button
        ResFont * font = dialogResources.getResFont ("kt-liberation");
        spText msg = new Text (font, default_value);

        // Create a new input dialog and attach the text to it
        ResAnim * sprite = dialogResources.getResAnim ("input");
        spInput input = new Input (sprite, msg, std::forward <std::function <void (std::string)>> (callback));
        input->setSize ({0.8f * getSize().x, 50});
        input->setPosition (0.1f * getSize().x, getSize().y / 5 + std::max (getSize().y / 10, input->getSize().y + 10) * buttons.size());
        addChild (input);
        buttons.emplace_back (input);
    }
}

/* Copyright © 2022 Aaron Alef */
