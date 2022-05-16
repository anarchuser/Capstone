#include "algebra.h"

bool closeEnough (double one, double two) {
    return std::abs (two - one) < EPSILON;
}

void makeEqual (double & one, double & two) {
    double mean = 0.5 * (one + two);
    one = mean;
    two = mean;
}

bool tryMakeEqual (double & one, double & two) {
    if (! closeEnough (one, two)) return false;
    makeEqual (one, two);
    return true;
}

/* Copyright © 2022 Aaron Alef */
