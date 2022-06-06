#ifndef CAPSTONE_DISTRIBUTION_H
#define CAPSTONE_DISTRIBUTION_H

#include <random>

/// Abstract base class for initialising random number distributions
struct Distribution {
    /// Create a new random number distribution with the given seed
    explicit Distribution (std::size_t seed);
    /// Make this class abstract
    virtual ~Distribution () = 0;

    /// Seed used to initialise the random number generator
    std::size_t const seed;
    /// Mersenne Twister pseudo-random number generator, initialised by the given seed and used to generate numbers
    std::mt19937 mt;
};

/** Returns a Seed guaranteed to be random */
std::size_t generateSeed();

#endif //CAPSTONE_DISTRIBUTION_H

/* Copyright © 2022 Aaron Alef */
