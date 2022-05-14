#include "KeyboardSpaceship.h"

namespace kt {

    KeyboardSpaceship * KeyboardSpaceship::instance = nullptr;

    KeyboardSpaceship::KeyboardSpaceship (World & world, Resources & res, Vector2 const & pos, float scale, std::string address)
            : Spaceship (world, res, pos, scale)
            , client {address}
            , sink {[&, this, address] () {
                instance = this;

                logs::messageln ("Connect to '%s'", address.c_str());
                setName (USERNAME);

                auto request = client.getMain <Synchro> ().joinRequest ();
                request.initOther().setValue (kj::heap <cg::SynchroImpl> (1));
                {
                    auto ship = request.initSpaceship();
                    ship.setUsername (USERNAME);
                    auto pos = ship.initPosition();
                    auto real_pos = getPhysicalPosition();
                    pos.setX (real_pos.x);
                    pos.setY (real_pos.y);
                    auto vel = ship.initVelocity();
                    auto real_vel = getPhysicalVelocity();
                    vel.setX (real_vel.x);
                    vel.setY (real_vel.y);
                    ship.setAngle (getRotation());
                }

                auto shipCB = kj::heap <cg::ShipCallbackImpl> ();
                shipCB->setOnSendSink (world.onSendSink);
                request.setShipCallback (kj::mv (shipCB));

                return request.send().wait (client.getWaitScope()).getItemSink();
            }()}
            {
        setAddColor (KEYBOARD_SPACESHIP_COLOR);

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
    }

    void KeyboardSpaceship::destroy () {
        sink.doneRequest().send().wait (client.getWaitScope());
        Spaceship::destroy();
        instance = nullptr;
    }
}

/* Copyright © 2022 Aaron Alef */
