#include "KeyboardSpaceship.h"

namespace kt {
    KeyboardSpaceship::KeyboardSpaceship (World & world, Resources * res, std::string const & username)
            : Spaceship (world, res, username)
            {
                setAddColor (KEYBOARD_SPACESHIP_COLOR);

                // Update directional commands on releasing corresponding keys
                listeners.push_back (getStage()->addEventListener (KeyEvent::KEY_UP, [this](Event * event) {
                    onSteeringEvent ((KeyEvent *) event);
                }));
                // Update directional commands on pressing corresponding keys
                listeners.push_back (getStage()->addEventListener (KeyEvent::KEY_DOWN, [this](Event * event) {
                    onSteeringEvent ((KeyEvent *) event);
                }));
            }

    void KeyboardSpaceship::setOnUpdate (cg::SendItemCallback && onUpdate) {
        this->onUpdate = onUpdate;
    }
    void KeyboardSpaceship::setOnDone (cg::DoneCallback && onDone) {
        this->onDone = onDone;
    }

    void KeyboardSpaceship::onSteeringEvent (ox::KeyEvent * event) {
        auto keysym = event->data->keysym;
        bool key_is_down = event->type == ox::KeyEvent::KEY_DOWN;

        ONCE (setAwake (true));

        // If WASD or arrow keys were pressed, update the directions queried
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
            onUpdate ({us.time, queried, getData()});
        } catch (std::bad_function_call & e) {
            logs::warning ("KeyboardSpaceship::onUpdate not configured");
        }
        Spaceship::update (us);
    }

    void KeyboardSpaceship::destroy () {
        try {
            onDone();
        } catch (std::bad_function_call & e) {
            logs::warning ("KeyboardSpaceship::onDone not configured");
        }
        Spaceship::destroy ();
    }

    kj::Own <cg::ShipSinkImpl> KeyboardSpaceship::getSink () {
        auto sink = Spaceship::getSink();
        sink->setOnDone ([]() { return kj::READY_NOW; });
        return sink;
    }
}

/* Copyright © 2022 Aaron Alef */
