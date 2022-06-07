#ifndef CAPSTONE_HASHEDRNG_H
#define CAPSTONE_HASHEDRNG_H

#include "../distribution/uniform/uniform.h"

#include "../bounds.h"

#include <unordered_map>
#include <memory>

/// Random number generator. Uses *one* seed to ensure consistency for all number ranges
class HashedRNG {
private:
    /// Map of random number generators previously used
    std::unordered_map <Bounds, Uniform> generators;

public:
    /// The One seed used to generate all random numbers
    unsigned int const seed;

    /// Create a new random number generator with random seed. Ideally, only one instance should ever be used
    HashedRNG ();
    /// Create a new random number with a specific seed. Ideally, only one instance should ever be used
    explicit HashedRNG (unsigned int seed);

    /// Create a new random double within the boundaries provided
    [[nodiscard]] double random (Bounds bounds);
};

#endif //CAPSTONE_HASHEDRNG_H

/* Copyright © 2022 Aaron Alef */
