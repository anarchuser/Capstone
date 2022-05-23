#include "Spaceship.h"

namespace kt {
    std::size_t Spaceship::ship_counter = 0;

    void Spaceship::resetCounter () {
        ship_counter = 0;
    }

    Spaceship::Spaceship (World & world, Resources * res, std::string const & username) {
        setName (username);

        attachTo (& world);
        setPosition (world.getSize() * 0.5);
        setRotation (1.5 * b2_pi);
        setAnchor (0.5, 0.5);
        setTouchChildrenEnabled (false);
        if (res)
            setResAnim (res->getResAnim ("spaceship"));

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = 0.5 * world.world_size;
        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
        bodyDef.angle = 1.5 * b2_pi;

        body = world.world.CreateBody (& bodyDef);
        setUserData (body);
        body->SetAwake (false);

        float scale = SPACESHIP_SCALE;
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

        scoreboard = new Text (res ? res->getResFont ("kt-liberation") : nullptr);
        scoreboard->setPosition (0.80 * getParent()->getSize().x, 10 + id * 40);
        getParent()->addChild (scoreboard);
        updateScoreboard();

        listeners.push_back (addEventListener (CollisionEvent::BEGIN, [this] (Event * event) {
            if (!body->IsAwake()) return;
//            spSprite other = safeCast <CollisionEvent *> (event)->other;
            --health;
            updateScoreboard();
            if (health <= 0) {
                logs::messageln ("Spaceship of '%s' crashed...", getName().c_str());
                destroy();
            }
        }));
    }

    void Spaceship::updateDirection (cg::Direction new_dir) {
        direction = new_dir;
    };

    void Spaceship::destroy () {
        for (auto listener: listeners)
            getStage ()->removeEventListener (listener);
        updateScoreboard ("dead");
        setAwake (false);
        if (body) body->GetUserData ().pointer = 0;
        body = nullptr;
        detach ();
    }

    void Spaceship::updateScoreboard (std::string msg) {
        if (msg.empty()) msg = std::to_string (health) + " hp";
        scoreboard->setText (getName() + ": " + msg);
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
        updateScoreboard ();

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

    b2Vec2 Spaceship::getPhysicalPosition () const {
        return body->GetPosition();
    }

    b2Vec2 Spaceship::getPhysicalVelocity () const {
        return body->GetLinearVelocity();
    }

    void Spaceship::setPhysicalTransform (b2Vec2 pos, float angle) {
        body->SetTransform (pos, angle);
    }

    void Spaceship::setPhysicalVelocity (b2Vec2 vel) {
        body->SetLinearVelocity (vel);
    }

    cg::Spaceship Spaceship::getData () const {
        return cg::Spaceship (
            getName(), {
                getPhysicalPosition().x,
                getPhysicalPosition().y
            }, {
                getPhysicalVelocity().x,
                getPhysicalVelocity().y
            },
            getRotation(),
            health
        );
    }
    void Spaceship::setData (cg::Spaceship const & spaceship) {
        setName (spaceship.username);
        setPhysicalTransform ({spaceship.position[0], spaceship.position[1]}, spaceship.angle);
        setPhysicalVelocity ({spaceship.velocity[0], spaceship.velocity[1]});
        health = spaceship.health;
        updateScoreboard ();
    }

    kj::Own <cg::ShipHandleImpl> Spaceship::getHandle () {
        setAwake (true);

        auto handle = kj::heap <cg::ShipHandleImpl> ();
        handle->setOnDone         (CLOSURE (this, & Spaceship::destroy));
        handle->setOnSendItem     (CLOSURE (this, & Spaceship::updateDirection));
        handle->setOnGetSpaceship (CLOSURE (this, & Spaceship::getData));
        handle->setOnSetSpaceship (CLOSURE (this, & Spaceship::setData));
        return handle;
    }
}

/* Copyright © 2022 Aaron Alef */
