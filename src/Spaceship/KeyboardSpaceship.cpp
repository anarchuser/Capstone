#include "KeyboardSpaceship.h"

namespace kt {
    KeyboardSpaceship::KeyboardSpaceship (World & world, Resources * res, std::string const & username, kj::WaitScope & waitScope)
            : Spaceship (world, res, username)
            , waitscope {waitScope}
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

    void KeyboardSpaceship::onSteeringEvent (ox::KeyEvent * event) {
        auto keysym = event->data->keysym;
        bool key_is_down = event->type == ox::KeyEvent::KEY_DOWN;

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

    void KeyboardSpaceship::update (UpdateState const & updateState) {
        try {
            onUpdate ({updateState.time, queried, getData()}).wait (waitscope);
        } catch (std::bad_function_call & e) {
            logs::warning ("KeyboardSpaceship::onUpdate not configured");
        }
        Spaceship::update (updateState);
    }
}

/* Copyright © 2022 Aaron Alef */
