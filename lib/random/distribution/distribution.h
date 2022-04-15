#ifndef CAPSTONE_DISTRIBUTION_H
#define CAPSTONE_DISTRIBUTION_H

#include <random>

struct Distribution {
    explicit Distribution(unsigned int SEED);
    virtual ~Distribution() = 0;

    const unsigned long SEED;
    std::mt19937 mt;
};

/** Returns a Seed guaranteed to be random */
unsigned int generateSeed();

#endif //CAPSTONE_DISTRIBUTION_H

/* Copyright © 2022 Aaron Alef */
