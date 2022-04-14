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

    b2PolygonShape triangle;
    b2Vec2 corners[3];
    corners[0].Set ( 0.0,  5.0);
    corners[1].Set (-2.5, -5.0);
    corners[2].Set ( 2.5, -5.0);
    triangle.Set(corners, sizeof (corners) / 8);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = & triangle;
    fixtureDef.density = 1.0;
    fixtureDef.friction = 1.0;

    body->CreateFixture (& fixtureDef);

    body->ApplyForceToCenter (b2Vec2(10, 10), false);

    ph::instance = this;

    ox::getStage()->addEventListener (ox::KeyEvent::KEY_DOWN, [=](Event * event) {
        ph::instance->onSteeringEvent ((ox::KeyEvent *) event);
    });
}

void ph::Spaceship::onSteeringEvent (ox::KeyEvent * event) {
    if (event->type != ox::KeyEvent::KEY_DOWN) return;

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
