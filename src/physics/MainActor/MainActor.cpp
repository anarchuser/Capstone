#include "MainActor.h"

ox::Resources ph::MainActor::gameResources = Resources();

ph::MainActor::MainActor(): world(b2Vec2_zero) {
    setSize(getStage()->getSize());

    auto res = ph::MainActor::gameResources.getResAnim ("spaceship");
    spSpaceship ship = new Spaceship (& world, res, getSize() / 2, 0.5);
    addChild (ship);

    ox::getStage()->addEventListener (ox::KeyEvent::KEY_DOWN, [=](Event * event) {
        auto * _event = safeCast <KeyEvent *> (event);

        switch (_event->data->keysym.scancode) {
            case SDL_SCANCODE_GRAVE:
                toggleDebugDraw();
                break;
            case SDL_SCANCODE_P:
                addPlanet();
                break;
        }
    });
}

ph::MainActor::~MainActor () noexcept = default;

Vector2 ph::MainActor::getRandomPos () {
    static VectorT2<Uniform> rng {{0, getSize().x}, {0, getSize().y}};
    return {float (rng.x.random()), float (rng.y.random())};
}

void ph::MainActor::addPlanet () {
    auto res = ph::MainActor::gameResources.getResAnim ("venus");
    spPlanet planet = new Planet (& world, res, getRandomPos(), 0.5);
    addChild (planet);

    logs::messageln ("Planet added at: %d, %d", planet->getPosition().x, planet->getPosition().y);
}

void ph::MainActor::doUpdate (const UpdateState & us) {
    world.Step (1 / 60.0, 8, 3);

    b2Body * body = world.GetBodyList();
    while (body) {
        auto * actor = (Actor *) body->GetUserData().pointer;
        b2Body * next = body->GetNext();

        if (actor) {
            b2Vec2 const & _pos = body->GetPosition();
            auto pos = ph::wrap (_pos);
            body->SetTransform (pos, body->GetAngle());
            actor->setPosition (ph::convert (pos));
            actor->setRotation (body->GetAngle());
        }

        body = next;
    }
}

void ph::MainActor::toggleDebugDraw () {
    if (_debugDraw) {
        logs::messageln ("disable debug draw");

        _debugDraw->detach();
        _debugDraw = nullptr;
        return;
    }

    logs::messageln ("enable debug draw");

    _debugDraw = new Box2DDraw;
    //TODO: put flags somewhere else
    _debugDraw->SetFlags (b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
    _debugDraw->attachTo (this);
    _debugDraw->setWorld (SCALE, & world);
    _debugDraw->setPriority (1);
}

/* Copyright © 2022 Aaron Alef */
