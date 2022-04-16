#include "distribution.h"

Distribution::Distribution(unsigned int seed) : mt(seed), seed{seed} {}

Distribution::~Distribution() = default;

unsigned int generateSeed() {
    std::random_device rd;
    unsigned int seed = rd();
    return seed;
}

/* Copyright © 2022 Aaron Alef */
