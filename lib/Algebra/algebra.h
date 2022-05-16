#ifndef CAPSTONE_ALGEBRA_H
#define CAPSTONE_ALGEBRA_H

// TODO: improve this
#define EPSILON 5e-7

#include <cmath>

bool closeEnough (double one, double two);

void makeEqual (double & one, double & two);

bool tryMakeEqual (double & one, double & two);

#endif //CAPSTONE_ALGEBRA_H

/* Copyright © 2022 Aaron Alef */
