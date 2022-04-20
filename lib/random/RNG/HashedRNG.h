#ifndef CAPSTONE_HASHEDRNG_H
#define CAPSTONE_HASHEDRNG_H

#include "../distribution/uniform/uniform.h"

#include "../bounds.h"

#include <unordered_map>
#include <memory>

class HashedRNG {
private:
    std::unordered_map <Bounds, Uniform> generators;

public:
    unsigned int const seed;

    HashedRNG ();
    explicit HashedRNG (unsigned int seed);

    [[nodiscard]] double random (Bounds bounds);
};

#endif //CAPSTONE_HASHEDRNG_H

/* Copyright © 2022 Aaron Alef */
