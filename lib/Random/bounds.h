#ifndef CAPSTONE_BOUNDS_H
#define CAPSTONE_BOUNDS_H

#include <stdexcept>
#include <functional>

/// Store lower inclusive and upper exclusive boundaries as container for interaction with random number generators
struct Bounds {
    double const lower, upper;

    inline Bounds (double lower, double upper)
            : lower {lower < upper ? lower : upper}
            , upper {lower < upper ? upper : lower}
            {
                if (!std::isnormal (lower) && lower != 0.0)
                    throw std::logic_error ("Real boundary expected; got " + std::to_string (lower));
                if (!std::isnormal (upper) && upper != 0.0)
                    throw std::logic_error ("Real boundary expected; got " + std::to_string (upper));
            }

    /// compare the given bounds for equality
    [[nodiscard]] inline bool operator == (Bounds other) const {
        return lower == other.lower && upper == other.upper;
    }

    /// Check whether the given value is within this object's boundaries
    [[nodiscard]] inline bool contains (double val) const {
        if (val == lower) return true;
        return val >= lower && val < upper;
    }
};

template <>
/// Allow the Bounds class to be used as key in unordered maps by providing a hash function for it
struct std::hash <Bounds> {
    inline std::size_t operator() (Bounds const & bounds) const noexcept {
        auto hash_lower = std::hash <double> {}(bounds.lower);
        auto hash_upper = std::hash <double> {}(bounds.upper);
        /* Combines the two individual hashes as proposed on the corresponding cppreference.com page:
         * https://en.cppreference.com/w/cpp/utility/hash (2022-06-06) */
        return hash_lower ^ (hash_upper << 1);
    }
};

#endif //CAPSTONE_BOUNDS_H

/* Copyright © 2022 Aaron Alef */
