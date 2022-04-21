#include "distribution.h"

Distribution::Distribution (std::size_t seed) : mt(seed), seed{seed} {}

Distribution::~Distribution () = default;

std::size_t generateSeed () {
    return std::random_device()();
}

/* Copyright © 2022 Aaron Alef */
