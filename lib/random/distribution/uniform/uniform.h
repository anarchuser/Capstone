#ifndef CAPSTONE_UNIFORM_H
#define CAPSTONE_UNIFORM_H

#include "../distribution.h"

/** Uniform probabilistic distribution, provides a very random but seeded random generator */
struct Uniform : Distribution {
    std::uniform_real_distribution<double> realDistribution;

    const double lowerBound;
    const double upperBound;

    /** Constructs a Random Generator generating numbers uniformly distributed. Randomly produces a seed
     *  @param lowerBound is the lowest possible number appearing
     *  @param upperBound is the lowest number above lowerBound impossible to appear */
    Uniform(double lowerBound, double upperBound) :
            Uniform(lowerBound, upperBound, generateSeed()) {}

    /** Constructs a Random Generator generating numbers uniformly distributed, based on the seed given
     * @param lowerBound is the lowest possible number appearing
     * @param upperBound is the lowers number above lowerBound impossible to appear
     * @param seed is determining the resulting numbers; the same seed always results in the same numbers */
    Uniform(double lowerBound, double upperBound, unsigned int seed) :
            Distribution(seed),
            lowerBound{lowerBound},
            upperBound{upperBound},
            realDistribution(lowerBound, upperBound) {}

    /** Returns a double randomly generated uniformly between the two bounds, based on the internal seed of the distribution */
    double rand() {return realDistribution(mt);}
};


#endif //CAPSTONE_UNIFORM_H

/* Copyright © 2022 Aaron Alef */
