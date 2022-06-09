#include "RNG.h"

RNG::RNG (): RNG (generateSeed ()) {}

RNG::RNG (unsigned int seed): seed {seed} {}

double RNG::random (Bounds bounds) {
    if (!generators.contains (bounds)) {
        // Add corresponding generator
        generators.emplace (bounds, Uniform (bounds, seed));
    }
    return generators.at(bounds).random();
}

/* Copyright © 2022 Aaron Alef */
