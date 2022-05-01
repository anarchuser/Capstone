#include "KeyboardSpaceship.h"

namespace kt {

    spKeyboardSpaceship KeyboardSpaceship::instance = nullptr;

    KeyboardSpaceship::KeyboardSpaceship (World & world, Resources & res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale) {
        // TODO: Is a special tag needed? Default is "Spaceship"
        // TODO: replace by player names?
//        setName ("KeyboardSpaceship");

        instance = this;

        getStage()->addEventListener (KeyEvent::KEY_UP, [](Event * event) {
            instance->onSteeringEvent ((KeyEvent *) event);
        });
        getStage()->addEventListener (KeyEvent::KEY_DOWN, [](Event * event) {
            instance->onSteeringEvent ((KeyEvent *) event);
        });
    }


    void KeyboardSpaceship::onSteeringEvent (ox::KeyEvent * event) {
        auto keysym = event->data->keysym;
        bool key_is_down = event->type == ox::KeyEvent::KEY_DOWN;

        ONCE (setAwake (true));

        switch (keysym.scancode) {
            case SDL_SCANCODE_UP: // accelerate
            case SDL_SCANCODE_W: // accelerate
                direction.accelerate = key_is_down;
                break;
            case SDL_SCANCODE_DOWN: // decelerate
            case SDL_SCANCODE_S: // decelerate
                direction.decelerate = key_is_down;
                break;
            case SDL_SCANCODE_LEFT: // turn left
            case SDL_SCANCODE_A: // turn left
                direction.rotateLeft = key_is_down;
                break;
            case SDL_SCANCODE_RIGHT: // turn right
            case SDL_SCANCODE_D: // turn right
                direction.rotateRight = key_is_down;
                break;
        }
    }
}

/* Copyright © 2022 Aaron Alef */
