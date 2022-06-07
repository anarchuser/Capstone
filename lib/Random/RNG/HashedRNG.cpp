#include "HashedRNG.h"

HashedRNG::HashedRNG (): HashedRNG (generateSeed ()) {}

HashedRNG::HashedRNG (unsigned int seed): seed {seed} {}

double HashedRNG::random (Bounds bounds) {
    if (!generators.contains (bounds)) {
        // Add corresponding generator
        generators.emplace (bounds, Uniform (bounds, seed));
    }
    return generators.at(bounds).random();
}

/* Copyright © 2022 Aaron Alef */
