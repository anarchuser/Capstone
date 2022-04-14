#include "MainActor.h"

ox::Resources ph::MainActor::gameResources = Resources();

ph::MainActor::MainActor(): world(b2Vec2_zero) {
    setSize(getStage()->getSize());

    auto res = ph::MainActor::gameResources.getResAnim ("spaceship");
    spSpaceship ship = new Spaceship (& world, res, getSize() / 2, 1);
    addChild (ship);
}

ph::MainActor::~MainActor () noexcept = default;

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

/* Copyright © 2022 Aaron Alef */
