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

    b2Body * body = world->CreateBody (& bodyDef);
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

    ox::getStage()->addEventListener (ox::KeyEvent::KEY_DOWN, [=](Event * event) {
        ph::instance->onSteeringEvent ((ox::KeyEvent *) event);
    });

    ph::instance = this;
}

void ph::Spaceship::onSteeringEvent (ox::KeyEvent * event) {
    auto keysym = event->data->keysym;
    auto * body = (b2Body *) getUserData();
    auto angle = body->GetAngle();
    auto direction = b2Vec2 (cos (angle), sin (angle));
    direction.Normalize();

    switch (keysym.scancode) {
        case SDL_SCANCODE_UP: // accelerate
        case SDL_SCANCODE_W: // accelerate
            body->ApplyLinearImpulseToCenter (FORCE * direction, true);
            break;
        case SDL_SCANCODE_DOWN: // decelerate
        case SDL_SCANCODE_S: // decelerate
            // Figure out a break mechanic
            body->ApplyLinearImpulseToCenter (-FORCE * direction, true);
            break;
        case SDL_SCANCODE_LEFT: // turn left
        case SDL_SCANCODE_A: // turn left
            body->ApplyAngularImpulse (-TORQUE, true);
            break;
        case SDL_SCANCODE_RIGHT: // turn right
        case SDL_SCANCODE_D: // turn right
            body->ApplyAngularImpulse (TORQUE, true);
            break;
        default:
            logs::messageln ("Unknown key: %d", keysym.scancode);
    }
}

/* Copyright © 2022 Aaron Alef */
