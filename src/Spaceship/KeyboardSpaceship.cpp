#include "KeyboardSpaceship.h"

namespace kt {

    KeyboardSpaceship * KeyboardSpaceship::instance = nullptr;

    KeyboardSpaceship::KeyboardSpaceship (World & world, Resources * res, std::string const & username)
            : Spaceship (world, res, username)
            {
                instance = this;
                setAddColor (KEYBOARD_SPACESHIP_COLOR);

                listeners.push_back (getStage()->addEventListener (KeyEvent::KEY_UP, [this](Event * event) {
                    onSteeringEvent ((KeyEvent *) event);
                }));
                listeners.push_back (getStage()->addEventListener (KeyEvent::KEY_DOWN, [this](Event * event) {
                    onSteeringEvent ((KeyEvent *) event);
                }));
            }

    void KeyboardSpaceship::setOnUpdate (std::function<void (cg::Direction)> && onUpdate) {
        this->onUpdate = onUpdate;
    }
    void KeyboardSpaceship::setOnDone (std::function<void ()> && onDone) {
        this->onDone = onDone;
    }

    void KeyboardSpaceship::onSteeringEvent (ox::KeyEvent * event) {
        auto keysym = event->data->keysym;
        bool key_is_down = event->type == ox::KeyEvent::KEY_DOWN;

        ONCE (setAwake (true));

        switch (keysym.scancode) {
            case SDL_SCANCODE_UP: // accelerate
            case SDL_SCANCODE_W: // accelerate
                queried.accelerate = key_is_down;
                break;
            case SDL_SCANCODE_DOWN: // decelerate
            case SDL_SCANCODE_S: // decelerate
                queried.decelerate = key_is_down;
                break;
            case SDL_SCANCODE_LEFT: // turn left
            case SDL_SCANCODE_A: // turn left
                queried.rotateLeft = key_is_down;
                break;
            case SDL_SCANCODE_RIGHT: // turn right
            case SDL_SCANCODE_D: // turn right
                queried.rotateRight = key_is_down;
                break;
        }
    }

    void KeyboardSpaceship::update (UpdateState const & us) {
        try {
            onUpdate(queried);
        } catch (std::bad_function_call & e) {
//            logs::warning ("KeyboardSpaceship::onUpdate not configured");
        }
        Spaceship::update (us);
    }

    void KeyboardSpaceship::destroy () {
        try {
            onDone();
        } catch (std::bad_function_call & e) {
//            logs::warning ("KeyboardSpaceship::onDone not configured");
        }
        Spaceship::destroy ();
        instance = nullptr;
    }

    kj::Own <cg::ShipHandleImpl> KeyboardSpaceship::getHandle () {
        setAwake (true);

        auto handle = kj::heap <cg::ShipHandleImpl> ();
        handle->setOnDone         (CLOSURE (this, & KeyboardSpaceship::destroy));
        handle->setOnSendItem     (CLOSURE (this, & KeyboardSpaceship::updateDirection));
        handle->setOnGetSpaceship (CLOSURE (this, & KeyboardSpaceship::getData));
        handle->setOnSetSpaceship (CLOSURE (this, & KeyboardSpaceship::setData));
        return handle;
    }
}

/* Copyright © 2022 Aaron Alef */
