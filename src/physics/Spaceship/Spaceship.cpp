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
    triangle[0].Set (-0.25f * scale, -0.25f * scale);
    triangle[1].Set ( 0.50f * scale,  0.00f * scale);
    triangle[2].Set (-0.25f * scale,  0.25f * scale);
    cone.Set(triangle, sizeof (triangle) / 8);

    b2FixtureDef coneFixture;
    coneFixture.shape = & cone;
    coneFixture.density = DENSITY_SPACESHIP;
    coneFixture.friction = 1.0;
    body->CreateFixture (& coneFixture);

    b2PolygonShape rear;
    b2Vec2 trapezoid[4];
    trapezoid[0].Set (-0.50f * scale,  0.50f * scale);
    trapezoid[1].Set (-0.25f * scale,  0.25f * scale);
    trapezoid[2].Set (-0.25f * scale, -0.25f * scale);
    trapezoid[3].Set (-0.50f * scale, -0.50f * scale);
    rear.Set(trapezoid, sizeof (trapezoid) / 8);

    b2FixtureDef rearFixture;
    rearFixture.shape = & rear;
    rearFixture.density = DENSITY_SPACESHIP;
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
#if BRAKE_TYPE == BRAKE_TYPE_UNDIRECTED
        auto velocity = body->GetLinearVelocity();
        velocity.Normalize();
        body->ApplyLinearImpulseToCenter (-FORCE * velocity, true);
#elif BRAKE_TYPE == BRAKE_TYPE_DIRECTED
        body->ApplyLinearImpulseToCenter (-FORCE * direction, true);
#endif
    } else if (accelerate) {
        body->ApplyLinearImpulseToCenter (FORCE * direction, true);
    }
    if (rotateRight && rotateLeft) {
        auto omega = body->GetAngularVelocity();
        if (omega < 0) {
            body->ApplyAngularImpulse (TORQUE, true);
        } else if (omega > 0) {
            body->ApplyAngularImpulse (-TORQUE, true);
        }
    } else {
#if TORQUE_TYPE == TORQUE_TYPE_CENTERED
        if (rotateLeft && !rotateRight) {
            body->ApplyAngularImpulse (-TORQUE, true);
        } else if (rotateRight && !rotateLeft) {
            body->ApplyAngularImpulse (TORQUE, true);
        }
#elif TORQUE_TYPE == TORQUE_TYPE_SIDE_IMPULSE
        if (rotateLeft) {
            auto leftRear = body->GetWorldPoint (b2Vec2 (-0.5, 0.25));
            body->ApplyLinearImpulse (TORQUE * direction, leftRear, true);
        }
        if (rotateRight) {
            auto rightRear = body->GetWorldPoint (b2Vec2 (-0.5, -0.25));
            body->ApplyLinearImpulse (TORQUE * direction, rightRear, true);
        }
#endif
    }

    Actor::update (us);
}

/* Copyright © 2022 Aaron Alef */
