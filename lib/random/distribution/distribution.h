#ifndef CAPSTONE_DISTRIBUTION_H
#define CAPSTONE_DISTRIBUTION_H

#include <random>

struct Distribution {
    explicit Distribution (std::size_t seed);
    virtual ~Distribution () = 0;

    std::size_t const seed;
    std::mt19937 mt;
};

/** Returns a Seed guaranteed to be random */
std::size_t generateSeed();

#endif //CAPSTONE_DISTRIBUTION_H

/* Copyright © 2022 Aaron Alef */
