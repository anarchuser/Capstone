#include "MainActor.h"

ox::Resources ph::MainActor::gameResources = Resources();

ph::MainActor::MainActor(): world(b2Vec2_zero) {
    setSize(getStage()->getSize());
}

ph::MainActor::~MainActor () noexcept = default;

/* Copyright © 2022 Aaron Alef */
