#ifndef CAPSTONE_UNIFORM_H
#define CAPSTONE_UNIFORM_H

#include "../distribution.h"
#include "../../bounds.h"

/** Uniform probabilistic distribution, provides a very random but seeded random generator */
struct Uniform : Distribution {
    /** Uniform real pseudo-random number distribution */
    std::uniform_real_distribution <double> realDistribution;

    /** Boundaries between numbers should be generated */
    Bounds const bounds;

    /** Constructs a Random Generator generating numbers uniformly distributed. Randomly produces a seed
     *  @param lowerBound is the lowest possible number appearing
     *  @param upperBound is the lowest number above lowerBound impossible to appear */
    explicit Uniform (Bounds bounds) :
            Uniform (bounds, generateSeed()) {}

    /** Constructs a Random Generator generating numbers uniformly distributed, based on the seed given
     * @param lowerBound is the lowest possible number appearing
     * @param upperBound is the lowers number above lowerBound impossible to appear
     * @param seed is determining the resulting numbers; the same seed always results in the same numbers */
    Uniform (Bounds bounds, unsigned int seed) :
            Distribution (seed),
            bounds {bounds},
            realDistribution (bounds.lower, bounds.upper) {}

    /** Returns a double randomly generated uniformly between the two bounds, based on the internal seed of the distribution */
    [[nodiscard]] double random() {
        return realDistribution (mt);
    }
};


#endif //CAPSTONE_UNIFORM_H

/* Copyright © 2022 Aaron Alef */
