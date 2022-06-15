#ifndef CAPSTONE_RNG_H
#define CAPSTONE_RNG_H

#include "distribution/uniform/uniform.h"

#include "bounds.h"

#include <unordered_map>
#include <memory>

/// Random number generator. Uses a single seed to ensure consistency for all number ranges.
/// It caches uniform distributions for all requested data ranges. WARNING: Can potentially get very large
class RNG {
private:
    /// Map of random number generators previously used
    std::unordered_map <Bounds, Uniform> generators;

public:
    /// The one seed used to generate all random numbers
    unsigned int const seed;

    /// Create a new random number generator with random seed
    RNG ();
    /// Create a new random number generator with a specific seed
    explicit RNG (unsigned int seed);

    /// Create a new random double within the boundaries provided
    [[nodiscard]] double random (Bounds bounds);
};

#endif //CAPSTONE_RNG_H

/* Copyright © 2022 Aaron Alef */
