#include "Spaceship.h"

namespace kt {
    std::size_t Spaceship::ship_counter = 0;

    Spaceship::Spaceship (World & world, Resources & res, Vector2 const & pos, float scale) {
        setName ("Spaceship");
        attachTo (& world);
        setPosition (pos);
        setRotation (1.5 * b2_pi);
        setResAnim (res.getResAnim ("spaceship"));
        setAnchor (0.5, 0.5);
        setTouchChildrenEnabled (false);

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = world.convert (pos);
        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
        bodyDef.angle = 1.5 * b2_pi;

        body = world.world.CreateBody (& bodyDef);
        setUserData (body);
        body->SetAwake (false);

        setScale (scale);

        b2PolygonShape cone;
        b2Vec2 triangle[3];
        triangle[0].Set (-0.25f * scale, -0.25f * scale);
        triangle[1].Set (0.50f * scale, 0.00f * scale);
        triangle[2].Set (-0.25f * scale, 0.25f * scale);
        cone.Set (triangle, sizeof (triangle) / 8);

        b2FixtureDef coneFixture;
        coneFixture.shape = & cone;
        coneFixture.density = SPACESHIP_DENSITY;
        coneFixture.isSensor = true;
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
        rearFixture.density = SPACESHIP_DENSITY;
        rearFixture.isSensor = true;
        body->CreateFixture (& rearFixture);

        scoreboard = new Text (res.getResFont ("kt-liberation"));
        scoreboard->setPosition (0.85 * getParent()->getSize().x, 10 + id * 40);
        getParent()->addChild (scoreboard);
        updateScoreboard();

        addEventListener (CollisionEvent::BEGIN, [this] (Event * event) {
            if (!body->IsAwake()) return;

            // Currently unused
            spSprite other = safeCast <CollisionEvent *> (event)->other;
            --health;
            updateScoreboard();
            if (health <= 0) {
                scoreboard->setText (std::to_string (id) + ": dead");
                detach();
            }
        });
    }

    void Spaceship::updateScoreboard () {
        scoreboard->setText (std::to_string (id) + ": " + std::to_string (health) + " hp");
    }

    void Spaceship::setAwake (bool awake) {
        auto * part = body->GetFixtureList();
        while (part) {
        part->SetSensor (!awake);
            part = part->GetNext();
        }
        body->SetAwake (awake);
    }

    void Spaceship::update (oxygine::UpdateState const & us) {
        // Update ship velocity
        if (direction.decelerate && !direction.accelerate) {
            // Decelerate spaceship. Works as universal brake, e.g., against gravity
            auto velocity = body->GetLinearVelocity ();
            velocity.Normalize ();
            body->ApplyLinearImpulseToCenter (-SPACESHIP_FORCE * velocity, true);

        } else if (direction.accelerate && !direction.decelerate) {
            // Accelerate spaceship in the direction it's facing
            auto angle = b2Vec2 (cos (body->GetAngle()), sin (body->GetAngle()));
            angle.Normalize ();
            body->ApplyLinearImpulseToCenter (SPACESHIP_FORCE * angle, true);
        }

        // Update angular velocity
        if (direction.rotateLeft && !direction.rotateRight) {
            // Rotate spaceship counter clock-wise around its origin
            body->ApplyAngularImpulse (-SPACESHIP_TORQUE, true);

        } else if (direction.rotateRight && !direction.rotateLeft) {
            // Rotate spaceship clock-wise around its origin
            body->ApplyAngularImpulse (SPACESHIP_TORQUE, true);

        } else {
            // Slow rotation by a fraction of its angular momentum
            auto omega = body->GetAngularVelocity ();
            body->ApplyAngularImpulse (- 0.1 * SPACESHIP_TORQUE * omega, false);
        }

        Actor::update (us);
    }
}

/* Copyright © 2022 Aaron Alef */
