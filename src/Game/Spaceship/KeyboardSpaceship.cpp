#include "KeyboardSpaceship.h"

namespace kt {

    spKeyboardSpaceship KeyboardSpaceship::instance = nullptr;

    KeyboardSpaceship::KeyboardSpaceship (World & world, oxygine::ResAnim * animation, Vector2 const & pos, float scale)
            : Spaceship (world, animation, pos, scale) {
        // TODO: Is a special tag needed? Default is "Spaceship"
//        setName ("KeyboardSpaceship");

        instance = this;
        setAwake (true);

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

        switch (keysym.scancode) {
            case SDL_SCANCODE_UP: // accelerate
            case SDL_SCANCODE_W: // accelerate
                accelerate = key_is_down;
                break;
            case SDL_SCANCODE_DOWN: // decelerate
            case SDL_SCANCODE_S: // decelerate
                decelerate = key_is_down;
                break;
            case SDL_SCANCODE_LEFT: // turn left
            case SDL_SCANCODE_A: // turn left
                rotateLeft = key_is_down;
                break;
            case SDL_SCANCODE_RIGHT: // turn right
            case SDL_SCANCODE_D: // turn right
                rotateRight = key_is_down;
                break;
        }
    }
}

/* Copyright © 2022 Aaron Alef */
