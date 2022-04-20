#ifndef CAPSTONE_BOUNDS_H
#define CAPSTONE_BOUNDS_H

#include <functional>

struct Bounds {
    double const lower, upper;

    Bounds (double lower, double upper): lower {lower}, upper {upper} {}

    bool operator == (Bounds other) const {
        return lower == other.lower && upper == other.upper;
    }
};

template <>
struct std::hash <Bounds> {
    std::size_t operator() (Bounds const & bounds) const noexcept {
        auto hash_lower = std::hash <double> {}(bounds.lower);
        auto hash_upper = std::hash <double> {}(bounds.upper);
        return hash_lower ^ (hash_upper << 1);
    }
};

#endif //CAPSTONE_BOUNDS_H

/* Copyright © 2022 Aaron Alef */
