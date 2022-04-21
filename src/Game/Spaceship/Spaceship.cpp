#include "Spaceship.h"

namespace kt {

    Spaceship::Spaceship (World & world, oxygine::ResAnim * animation, Vector2 const & pos, float scale) {
        setResAnim (animation);
        setAnchor (0.5, 0.5);
        setTouchChildrenEnabled (false);

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = world.convert (pos);
        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
        bodyDef.angle = 1.5 * b2_pi;

        body = world.world.CreateBody (& bodyDef);
        setUserData (body);

        setScale (scale);

        b2PolygonShape cone;
        b2Vec2 triangle[3];
        triangle[0].Set (-0.25f * scale, -0.25f * scale);
        triangle[1].Set (0.50f * scale, 0.00f * scale);
        triangle[2].Set (-0.25f * scale, 0.25f * scale);
        cone.Set (triangle, sizeof (triangle) / 8);

        b2FixtureDef coneFixture;
        coneFixture.shape = & cone;
        coneFixture.density = 1.0;
        body->CreateFixture (& coneFixture);

        b2PolygonShape rear;
        b2Vec2 trapezoid[4];
        trapezoid[0].Set (-0.50f * scale, 0.50f * scale);
        trapezoid[1].Set (-0.25f * scale, 0.25f * scale);
        trapezoid[2].Set (-0.25f * scale, -0.25f * scale);
        trapezoid[3].Set (-0.50f * scale, -0.50f * scale);
        rear.Set (trapezoid, sizeof (trapezoid) / 8);

        b2FixtureDef rearFixture;
        rearFixture.shape = & rear;
        rearFixture.density = 1.0;
        body->CreateFixture (& rearFixture);
    }

    void Spaceship::update (oxygine::UpdateState const & us) {
        auto angle = body->GetAngle ();
        auto direction = b2Vec2 (cos (angle), sin (angle));
        direction.Normalize ();

        if (decelerate) {
            auto velocity = body->GetLinearVelocity ();
            velocity.Normalize ();
            body->ApplyLinearImpulseToCenter (- SPACESHIP_FORCE * velocity, true);
        } else if (accelerate) {
            body->ApplyLinearImpulseToCenter (SPACESHIP_FORCE * direction, true);
        }
        auto omega = body->GetAngularVelocity ();
        if (omega < 0) {
            body->ApplyAngularImpulse (0.5 * SPACESHIP_TORQUE, true);
        } else if (omega > 0) {
            body->ApplyAngularImpulse (-0.5 * SPACESHIP_TORQUE, true);
        }
        if (rotateLeft && !rotateRight) {
            body->ApplyAngularImpulse (- SPACESHIP_TORQUE, true);
        } else if (rotateRight && !rotateLeft) {
            body->ApplyAngularImpulse (SPACESHIP_TORQUE, true);
        }

        Actor::update (us);
    }
}

/* Copyright © 2022 Aaron Alef */
