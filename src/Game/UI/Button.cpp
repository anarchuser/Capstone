#include "Button.h"

namespace kt {
    Button::Button (ResAnim * animation, spText message, std::function<void (Event *)> && onAction):
            onAction{std::move (onAction)} {

        setGuides (15, 15, 15, 15);

        message->setPosition (20, 5);
        message->setColor (Color (0, 0, 0, 255));
        addChild (message);

        addClickListener (CLOSURE (this, & Button::trigger));
        addEventListener (TouchEvent::OVER, CLOSURE (this, & Button::onMouseOver));
        addEventListener (TouchEvent::OUTX, CLOSURE (this, & Button::onMouseOut));
    }

    void Button::trigger (Event * event) {
        event->stopsImmediatePropagation = true;
        addTween (Sprite::TweenAnim (getResAnim()), 200);
        onAction (event);
    }

    void Button::onMouseOver (Event * event) {
        event->stopsImmediatePropagation = true;
        addTween (Sprite::TweenAddColor (Color (32, 32, 32, 0)), 100);
    }
    void Button::onMouseOut  (Event * event) {
        event->stopsImmediatePropagation = true;
        addTween (Sprite::TweenAddColor (Color (0, 0, 0, 0)), 100);
    }
}

/* Copyright © 2022 Aaron Alef */
