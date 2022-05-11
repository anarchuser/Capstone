#include "KeyboardSpaceship.h"

namespace kt {

    KeyboardSpaceship * KeyboardSpaceship::instance = nullptr;

    KeyboardSpaceship::KeyboardSpaceship (World & world, Resources & res, Vector2 const & pos, float scale)
            : Spaceship (world, res, pos, scale)
            , client {SERVER_FULL_ADDRESS}
            , sink {[this, &world] () {
                logs::messageln ("Connect to '%s'", SERVER_FULL_ADDRESS.c_str());
                setName (USERNAME);

                auto request = client.getMain <Synchro> ().joinRequest ();
                request.initOther().setValue (kj::heap <cg::SynchroImpl> (1));
                request.setUsername (USERNAME);

                auto shipCB = kj::heap <cg::ShipCallbackImpl> ();
                shipCB->setOnSendSink (world.onSendSink);
                request.setShipCallback (kj::mv (shipCB));

                return request.send().wait (client.getWaitScope()).getItemSink();
            }()}
            {
        setAddColor (KEYBOARD_SPACESHIP_COLOR);

        instance = this;

        // TODO: remove this
        setAwake (true);

        listeners.push_back (getStage()->addEventListener (KeyEvent::KEY_UP, [](Event * event) {
            instance->onSteeringEvent ((KeyEvent *) event);
        }));
        listeners.push_back (getStage()->addEventListener (KeyEvent::KEY_DOWN, [](Event * event) {
            instance->onSteeringEvent ((KeyEvent *) event);
        }));
    }

    KeyboardSpaceship::~KeyboardSpaceship () noexcept {
        sink.doneRequest().send().wait (client.getWaitScope());
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
        auto request = sink.sendItemRequest ();
        auto dir = request.initItem().initDirection();
        dir.setAccelerate (queried.accelerate);
        dir.setDecelerate (queried.decelerate);
        dir.setRotateLeft (queried.rotateLeft);
        dir.setRotateRight (queried.rotateRight);
        auto promise = request.send();
        Spaceship::update (us);
        promise.wait (client.getWaitScope());
//        std::cout << "q-d:\t" << (int) * (char *) & queried << " - " << (int) * (char *) & direction << std::endl;
    }

    void KeyboardSpaceship::destroy () {
        sink.doneRequest().send().wait (client.getWaitScope());
        Spaceship::destroy();
        instance = nullptr;
    }
}

/* Copyright © 2022 Aaron Alef */
