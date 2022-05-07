#include "Input.h"

namespace kt {
    Input::Input (ResAnim * animation, spText message, std::function<void (std::string)> && onAction):
            onEnter {std::move (onEnter)} {

        setResAnim (animation);

        message->setPosition (20, 5);
        message->setColor (Color (0, 0, 0, 255));
        addChild (message);

        addClickListener (CLOSURE (this, & Input::onClick));
        addEventListener (TouchEvent::OVER, CLOSURE (this, & Input::onMouseOver));
        addEventListener (TouchEvent::OUTX, CLOSURE (this, & Input::onMouseOut));
    }

    void Input::onClick (Event * event) {

    }
    void Input::onMouseOver (Event * event) {
        event->stopImmediatePropagation();
        addTween (Sprite::TweenAddColor (Color (32, 32, 32, 0)), 100);
    }
    void Input::onMouseOut  (Event * event) {
        event->stopImmediatePropagation();
        this->removeTweens (false);
        addTween (Sprite::TweenAddColor (Color (0, 0, 0, 0)), 100);
    }
}

/* Copyright © 2022 Aaron Alef */
