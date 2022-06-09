#include "Spaceship.h"

namespace kt {
    std::size_t Spaceship::ship_counter = 0;

    void Spaceship::resetCounter () {
        ship_counter = 0;
    }

    Spaceship::Spaceship (World & world, Resources * res, std::string const & username) {
        setName (username);

        // Create and position the sprite appropriately
        attachTo (& world);
        setPosition (world.getSize() * 0.5);
        setRotation (1.5 * b2_pi);
        setAnchor (0.5, 0.5);
        setTouchChildrenEnabled (false);
        if (res)
            setResAnim (res->getResAnim ("spaceship"));

        // Define physical properties
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = 0.5 * world.world_size;
        bodyDef.angle = 1.5 * b2_pi;
        // Store a reference to the actor in the body
        bodyDef.userData.pointer = (uintptr_t) this;

        // Create the physical body in the world, asleep
        body = world.world.CreateBody (& bodyDef);
        body->SetAwake (false);

        // Store a reference to the body in the actor
        setUserData (body);

        static float const scale = SPACESHIP_SCALE;
        setScale (scale);

        // Define the shape of the spaceship
        // - the cone, consisting of a triangle
        b2PolygonShape cone;
        b2Vec2 triangle[3];
        triangle[0].Set (-0.25f * scale, -0.25f * scale);
        triangle[1].Set (0.50f * scale, 0.00f * scale);
        triangle[2].Set (-0.25f * scale, 0.25f * scale);
        cone.Set (triangle, sizeof (triangle) / 8);

        // Attach the cone to the body
        b2FixtureDef coneFixture;
        coneFixture.shape = & cone;
        coneFixture.density = SPACESHIP_DENSITY;
        coneFixture.isSensor = true;
        body->CreateFixture (& coneFixture);

        // - the rear, consisting of a regular trapeze
        b2PolygonShape rear;
        b2Vec2 trapezoid[4];
        trapezoid[0].Set (-0.50f * scale, 0.50f * scale);
        trapezoid[1].Set (-0.25f * scale, 0.25f * scale);
        trapezoid[2].Set (-0.25f * scale, -0.25f * scale);
        trapezoid[3].Set (-0.50f * scale, -0.50f * scale);
        rear.Set (trapezoid, sizeof (trapezoid) / 8);

        // Attach the rear to the body
        b2FixtureDef rearFixture;
        rearFixture.shape = & rear;
        rearFixture.density = SPACESHIP_DENSITY;
        rearFixture.isSensor = true;
        body->CreateFixture (& rearFixture);

        // Create a scoreboard displaying name, hp/status, and latency to its origin
        scoreboard = new Text (res ? res->getResFont ("kt-liberation") : nullptr);
        scoreboard->setPosition (0.70 * getParent()->getSize().x, 10 + id * 40);
        getParent()->addChild (scoreboard);
        updateScoreboard();

        listeners.push_back (addEventListener (CollisionEvent::BEGIN, [this] (Event * event) {
            if (!body->IsAwake()) return;
            // Update health and then the scoreboard
            --health;
            updatePing();
            if (health <= 0) destroy();
        }));
    }

    void Spaceship::setOnUpdate (cg::SendItemCallback && onUpdate) {
        this->onUpdate = std::move (onUpdate);

        // OnUpdate must be set before the ship can receive updates, so wake them up then
        setAwake (true);
    }
    void Spaceship::setOnDone (cg::DoneCallback && onDone) {
        this->onDone = std::move (onDone);
    }

    void Spaceship::updateDirection (cg::Direction new_dir) {
        direction = new_dir;
    }

    Spaceship::~Spaceship() noexcept {
        try {
            onDone();
        } catch (std::bad_function_call & e) {
            logs::warning ("Spaceship destroyed without done callback registered");
        }

        // Stop listening to anything
        for (auto listener: listeners) getStage ()->removeEventListener (listener);
        listeners.clear();

        remote.reset();

        // Update ship status
        updateScoreboard ("dead");

        // Put ship to sleep
        setAwake (false);

        // Remove the physical body and detach the ship from the game
        if (body) body->GetUserData().pointer = 0;
        body = nullptr;
        detach ();
    }

    void Spaceship::destroy () {
        Spaceship::~Spaceship();
    }

    void Spaceship::updateScoreboard (std::string const & msg, long ping) {
        // Start HUD with ship username
        auto text = getName() + ": ";
        // Either set the given text
        if (!msg.empty()) text += msg;
        // Or set health and ping instead
        else {
            text += std::to_string (health) + " hp";
            text += " (" + std::to_string (ping) + ")";
        }
        scoreboard->setText (text);
    }

    void Spaceship::setAwake (bool awake) {
        // Make every fixture a sensor -> remove collision from the body
        auto * part = body->GetFixtureList();
        while (part) {
            part->SetSensor (!awake);
            part = part->GetNext();
        }
        // Stop planets from applying force to this body
        body->SetAwake (awake);
    }

    void Spaceship::update (oxygine::UpdateState const & updateState) {
        // Update each ship's ping roughly once a second
        if (updateState.time % int (1000 / FPS) == 0) updatePing ();

        // Update ship velocity depending on whether W or S is pressed
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

        // Update angular velocity depending on whether A or D is pressed
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

        Actor::update (updateState);
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
        setPhysicalTransform ({spaceship.position.x, spaceship.position.y}, spaceship.angle);
        setPhysicalVelocity ({spaceship.velocity.x, spaceship.velocity.y});
        auto tmp = health;
        health = spaceship.health;
        if (health != tmp) updateScoreboard ();
    }

    kj::Own <cg::ShipSinkImpl> Spaceship::getSink () {
        auto sink = kj::heap <cg::ShipSinkImpl> ();
        setAwake (true);
        sink->setOnDone (LAMBDA (destroy));
        sink->setOnSendItem ([this] (cg::Item const & item) {
            updateDirection (item.direction);
            setData (item.spaceship);
        });
        sink->setOnGetShip (LAMBDA (getData));
        return sink;
    }

    kj::Own <cg::ShipHandleImpl> Spaceship::getHandle () {
        auto handle = kj::heap <cg::ShipHandleImpl> ();
        handle->setOnGetSink (LAMBDA (getSink));
        handle->setOnGetShip (LAMBDA (getData));
        handle->setOnSetShip (LAMBDA (setData));
        return handle;
    }
    void Spaceship::setHandle (cg::ShipHandle_t && handle) {
        remote = std::move (handle);
    }

    void Spaceship::updatePing () {
        // If we don't receive update events yet, display our ping as -1
        if (!remote) return updateScoreboard ();

        // Measure the time it takes for a ping to return
        auto start = std::chrono::high_resolution_clock::now();
        remote->pingRequest().send().ignoreResult().then ([this, start] () {
            auto end = std::chrono::high_resolution_clock::now();
            auto latency = (std::chrono::duration_cast<std::chrono::milliseconds> (end - start)).count();
            updateScoreboard ("", latency);
        }).detach ([this] (kj::Exception && e) {
            logs::warning (e.getDescription().cStr());
            detach();
        });
    }
}

/* Copyright © 2022 Aaron Alef */
