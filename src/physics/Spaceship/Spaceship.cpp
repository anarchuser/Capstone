#include "Spaceship.h"

ph::spSpaceship instance = nullptr;

ph::Spaceship::Spaceship (b2World * world, oxygine::ResAnim * animation, Vector2 const & pos, float scale) {
    setResAnim (animation);
    setAnchor (0.5, 0.5);
    setTouchChildrenEnabled (false);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = ph::convert (pos);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    bodyDef.angle = - 0.5 * b2_pi;

    body = world->CreateBody (& bodyDef);
    setUserData (body);

    setScale (scale);

    b2PolygonShape cone;
    b2Vec2 triangle[3];
    triangle[0].Set (-0.25, -0.25);
    triangle[1].Set ( 0.50,  0.00);
    triangle[2].Set (-0.25,  0.25);
    cone.Set(triangle, sizeof (triangle) / 8);

    b2FixtureDef coneFixture;
    coneFixture.shape = & cone;
    coneFixture.density = 1.0;
    coneFixture.friction = 1.0;
    body->CreateFixture (& coneFixture);

    b2PolygonShape rear;
    b2Vec2 trapezoid[4];
    trapezoid[0].Set (-0.50,  0.50);
    trapezoid[1].Set (-0.25,  0.25);
    trapezoid[2].Set (-0.25, -0.25);
    trapezoid[3].Set (-0.50, -0.50);
    rear.Set(trapezoid, sizeof (trapezoid) / 8);

    b2FixtureDef rearFixture;
    rearFixture.shape = & rear;
    rearFixture.density = 1.0;
    rearFixture.friction = 1.0;
    body->CreateFixture (& rearFixture);

    ox::getStage()->addEventListener (ox::KeyEvent::KEY_UP, [=](Event * event) {
        ph::instance->onSteeringEvent ((ox::KeyEvent *) event);
    });
    ox::getStage()->addEventListener (ox::KeyEvent::KEY_DOWN, [=](Event * event) {
        ph::instance->onSteeringEvent ((ox::KeyEvent *) event);
    });

    ph::instance = this;
}

void ph::Spaceship::onSteeringEvent (ox::KeyEvent * event) {
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
        default:
            logs::messageln ("Unknown key: %d", keysym.scancode);
    }
}

void ph::Spaceship::update (oxygine::UpdateState const & us) {
    auto angle = body->GetAngle();
    auto direction = b2Vec2 (cos (angle), sin (angle));
    direction.Normalize();

    if (decelerate) {
        //TODO: Figure out a break mechanic
        body->ApplyLinearImpulseToCenter (-FORCE * direction, true);
    } else if (accelerate) {
        body->ApplyLinearImpulseToCenter (FORCE * direction, true);
    }
    if (rotateLeft && !rotateRight) {
        //TODO: Don't rotate but apply impulse to left half of ship instead
        body->ApplyAngularImpulse (-TORQUE, true);
    } else if (rotateRight && ! rotateLeft) {
        //TODO: Don't rotate but apply impulse to right half of ship instead
        body->ApplyAngularImpulse (TORQUE, true);
    }

    Actor::update (us);
}

/* Copyright © 2022 Aaron Alef */
