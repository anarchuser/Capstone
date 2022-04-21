#include "KeyboardSpaceship.h"

namespace kt {

    KeyboardSpaceship * KeyboardSpaceship::instance = nullptr;

    KeyboardSpaceship::KeyboardSpaceship (World & world, oxygine::ResAnim * animation, Vector2 const & pos, float scale)
            : Spaceship (world, animation, pos, scale) {

        instance = this;

        auto * part = body->GetFixtureList();
        while (part) {
            part->SetSensor (false);
            part = part->GetNext();
        }
        body->SetAwake (false);

        getStage()->addEventListener (ox::KeyEvent::KEY_UP, [=](Event * event) {
            instance->onSteeringEvent ((ox::KeyEvent *) event);
        });
        getStage()->addEventListener (ox::KeyEvent::KEY_DOWN, [=](Event * event) {
            instance->onSteeringEvent ((ox::KeyEvent *) event);
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
