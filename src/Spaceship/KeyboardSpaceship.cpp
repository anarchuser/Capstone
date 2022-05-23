#include "KeyboardSpaceship.h"

namespace kt {

    KeyboardSpaceship * KeyboardSpaceship::instance = nullptr;

    KeyboardSpaceship::KeyboardSpaceship (World & world, Resources * res, ::Backend::ShipRegistrar::Client & registrar, kj::WaitScope & waitscope)
            : Spaceship (world, res)
            , waitscope {waitscope}
            , handle {[&] () -> Backend::ShipHandle::Client {
                instance = this;

                logs::messageln ("Connect instance '%p' to backend", this);
                setName (USERNAME);

                auto request = registrar.registerShipRequest();
                getData().initialise (request.initSpaceship());
                request.setHandle (getHandle());
                return request.send().wait (waitscope).getRemote();
            }()}
            {
        setAddColor (KEYBOARD_SPACESHIP_COLOR);

        registrar.whenResolved().then ([this] () { destroy(); });

        listeners.push_back (getStage()->addEventListener (KeyEvent::KEY_UP, [this](Event * event) {
            onSteeringEvent ((KeyEvent *) event);
        }));
        listeners.push_back (getStage()->addEventListener (KeyEvent::KEY_DOWN, [this](Event * event) {
            onSteeringEvent ((KeyEvent *) event);
        }));
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
        auto request = handle.sendItemRequest ();
        queried.initialise (request.initItem().initDirection());

        try {
            auto promise = request.send ();
            Spaceship::update (us);
            promise.wait (waitscope);
        } catch (std::exception & e) {
            destroy();
        }
    }

    void KeyboardSpaceship::destroy () {
        logs::messageln ("KeyboardSpaceship::destroy");
        try {
            handle.doneRequest ().send().wait (waitscope);
        } catch (...) {}
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
