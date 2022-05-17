#ifndef CAPSTONE_BOUNDS_H
#define CAPSTONE_BOUNDS_H

#include <stdexcept>
#include <functional>

struct Bounds {
    double const lower, upper;

    Bounds (double lower, double upper)
            : lower {lower < upper ? lower : upper}
            , upper {lower < upper ? upper : lower}
            {
                if (!std::isnormal (lower) && lower != 0.0)
                    throw std::logic_error ("Real boundary expected; got " + std::to_string (lower));
                if (!std::isnormal (upper) && upper != 0.0)
                    throw std::logic_error ("Real boundary expected; got " + std::to_string (upper));
            }

    [[nodiscard]] bool operator == (Bounds other) const {
        return lower == other.lower && upper == other.upper;
    }

    [[nodiscard]] bool contains (double val) const {
        if (val == lower) return true;
        return val >= lower && val < upper;
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
