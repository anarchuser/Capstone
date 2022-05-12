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
                        break;
                    case SDL_SCANCODE_PERIOD:
                        message->setText (msg += '.');
                        break;
                    case SDL_SCANCODE_0:
                        // special case since scancodes go from 1 to 9 to 0 while ASCII goes from 0 to 9
                        message->setText (msg += '0');
                    default:
                        if (keysym.scancode >= SDL_SCANCODE_A && keysym.scancode <= SDL_SCANCODE_Z) {
                            message->setText (msg += keysym.scancode - SDL_SCANCODE_A + 'A');
                        } else if (keysym.scancode >= SDL_SCANCODE_1 && keysym.scancode <= SDL_SCANCODE_9) {
                            message->setText (msg += keysym.scancode - SDL_SCANCODE_1 + '1');
                        }
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
