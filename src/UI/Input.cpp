#include "Input.h"

namespace kt {
    Input::Input (ResAnim * animation, spText message, std::function <void (std::string)> && onEnter)
            : onEnter {std::move (onEnter)}
            , message {message}
            {

        setResAnim (animation);

        message->setPosition (20, 5);
        message->setColor (Color (0, 0, 0, 255));
        addChild (message);

        addClickListener (CLOSURE (this, & Input::onClick));
        addEventListener (TouchEvent::OVER, CLOSURE (this, & Input::onMouseOver));
        addEventListener (TouchEvent::OUTX, CLOSURE (this, & Input::onMouseOut));
    }

    void Input::onClick (Event * event) {
        event->stopImmediatePropagation();
        getStage()->addEventListener (KeyEvent::KEY_DOWN, [this] (Event * event) {
            event->stopImmediatePropagation();
            auto * keyEvent = (KeyEvent *) event;
            auto keysym = keyEvent->data->keysym;
            static bool first = true;
            auto msg = message->getText ();
            if (keysym.scancode == SDL_SCANCODE_KP_ENTER || keysym.scancode == SDL_SCANCODE_RETURN) {
                logs::messageln ("Enter pressed");
                onEnter (msg);
            } else {
                switch (keysym.scancode) {
                    case SDL_SCANCODE_BACKSPACE:
                        message->setText (msg.substr (0, msg.size () - 1));
                }
            }
            first = false;
        });
    }
    void Input::onMouseOver (Event * event) {
        event->stopImmediatePropagation();
        addTween (Sprite::TweenAddColor (Color (32, 32, 32, 0)), 100);
    }
    void Input::onMouseOut  (Event * event) {
        event->stopImmediatePropagation();
        removeTweens (false);
        removeEventListener (listener);
        addTween (Sprite::TweenAddColor (Color (0, 0, 0, 0)), 100);
    }
}

/* Copyright © 2022 Aaron Alef */
