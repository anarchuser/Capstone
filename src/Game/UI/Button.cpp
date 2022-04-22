#include "Button.h"

namespace kt {
    Button::Button (ResAnim * animation, spText message, std::function<void (Event const *)> && onAction):
            onAction{std::move (onAction)} {

        setGuides (20, 20, 20, 20);

        message->setPosition (getPosition().x + 20, getPosition().y + 5);
        addChild (message);

        addClickListener (CLOSURE (this, & Button::trigger));
        addEventListener (TouchEvent::OVER, CLOSURE (this, & Button::onMouseOver));
        addEventListener (TouchEvent::OUTX, CLOSURE (this, & Button::onMouseOut));
    }

    void Button::trigger (Event const * event) {
        addTween (Sprite::TweenAnim (getResAnim()), 200);
        onAction (event);
    }

    void Button::onMouseOver (Event const * event) {
        addTween (Sprite::TweenAddColor (Color (64, 64, 64, 0)), 100);
    }
    void Button::onMouseOut  (Event const * event) {
        addTween (Sprite::TweenAddColor (Color (0, 0, 0, 0)), 100);
    }
}

/* Copyright © 2022 Aaron Alef */
